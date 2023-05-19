#include "stdafx.h"
#include "SpehsEngine/Net/SocketUDP.h"

#include <assert.h>
#include <iostream>
#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/IOServiceUtilityFunctions.h"
#include "SpehsEngine/Core/RAIIVariableSetter.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info("SocketUDP(" + getLocalPort().toString() + "): " + message); \
}


namespace
{
	typedef uint16_t SizeType;

#ifdef SE_FINAL_RELEASE
	static const se::time::Time connectionTimeout = se::time::fromSeconds(5.0f);
#else
	static const se::time::Time connectionTimeout = se::time::fromSeconds(10000.0f);
#endif
}

namespace se
{
	namespace net
	{
		struct HandshakeUDP
		{
			bool isValid() const
			{
				return handshakeVersion == currentVersion;
			}

			static const uint16_t currentVersion = 1;
			uint16_t handshakeVersion = currentVersion;
			uint64_t key = 0;
		};
	}

	template<> template<typename S, typename T>
	static bool se::Serial<net::HandshakeUDP>::serial(S& _serial, T _value)
	{
		se_serial(_serial, _value.handshakeVersion, "handshakeVersion");
		se_serial(_serial, _value.key, "key");
		return true;
	}

	extern std::string workingDirectory;
	namespace net
	{
		SocketUDP::SocketUDP(IOService& _ioService)
			: sharedImpl(new SharedImpl(_ioService))
		{
			sharedImpl->socketUDP = this;
		}

		SocketUDP::~SocketUDP()
		{
			close();
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			se_assert(sharedImpl->socketUDP);
			sharedImpl->socketUDP = nullptr;
		}

		bool SocketUDP::open()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->socket.is_open())
			{
				return true;
			}
			boost::system::error_code error;
			sharedImpl->socket.open(boost::asio::ip::udp::v4(), error);
			if (error)
			{
				DEBUG_LOG(1, "failed to open. Boost asio error: " + error.message());
				return false;
			}
			return true;
		}

		void SocketUDP::close()
		{
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				if (sharedImpl->socket.is_open())
				{
					sharedImpl->onReceiveCallback = std::function<void(BinaryReader&, const boost::asio::ip::udp::endpoint&)>();
					boost::system::error_code error;
					sharedImpl->socket.shutdown(boost::asio::socket_base::shutdown_both, error);
					if (error)
					{
						DEBUG_LOG(1, "failed to shutdown. Boost asio error: " + error.message());
					}
					sharedImpl->socket.close(error);
					if (error)
					{
						DEBUG_LOG(1, "failed to close. Boost asio error: " + error.message());
					}
					DEBUG_LOG(1, "failed to close.");
				}
				clearReceivedPackets();
			}
			while (isReceiving())
			{
				//Blocks
			}
		}

		bool SocketUDP::bind(const Port& port)
		{
			if (!isOpen())
			{
				if (!open())
				{
					if (sharedImpl->debugLogLevel >= 1)
					{
						DEBUG_LOG(1, "failed to bind. Socket was not open and could not be opened.");
					}
					return false;
				}
				return false;
			}
			boost::system::error_code error;
			const boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), port.value);

			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			sharedImpl->socket.bind(endpoint, error);
			if (error)
			{
				DEBUG_LOG(1, "failed to bind. Boost asio error: " + error.message());
				return false;
			}
			DEBUG_LOG(1, "binding successful.");
			return true;
		}

		bool SocketUDP::connect(const Endpoint& remoteEndpoint)
		{
			if (!isOpen())
			{
				if (!open())
				{
					DEBUG_LOG(1, "failed to connect socket. Open failed.");
					return false;
				}
			}
			if (!remoteEndpoint)
			{
				DEBUG_LOG(1, "failed to connect. Endpoint is invalid: " + remoteEndpoint.toString());
				return false;
			}

			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				boost::asio::ip::udp::resolver resolverUDP(getImplementationRef(sharedImpl->ioService));
				const boost::asio::ip::udp::resolver::query queryUDP(boost::asio::ip::udp::v4(), remoteEndpoint.address.toString(), remoteEndpoint.port.toString());
				boost::asio::ip::udp::resolver::iterator it = resolverUDP.resolve(queryUDP);
				const boost::asio::ip::udp::resolver::iterator end;
				if (it == end)
				{
					DEBUG_LOG(1, "failed to connect. Endpoint resolution has failed.");
					return false;
				}
				while (it != end)
				{
					const boost::asio::ip::udp::endpoint remoteAsioEndpoint = *it++;
					boost::system::error_code error;
					sharedImpl->socket.connect(remoteAsioEndpoint, error);
					if (error)
					{
						DEBUG_LOG(1, "connect() failed. Boost asio error: " + std::to_string(error.value()) + ": " + error.message());
						if (it == end)
						{
							DEBUG_LOG(1, "connect failed. All available endpoints were iterated, connection cannot be established.");
							return false;
						}
					}
					else
					{
						se_assert(remoteAsioEndpoint.address().to_v4().to_ulong() != 0);
						se_assert(remoteAsioEndpoint.port() != 0);
						sharedImpl->connectedEndpoint = remoteAsioEndpoint;
					}
				}
			}
			
			/* Send out a keyed handshake and wait for a response. */
			bool handshakeSuccess = false;
			const size_t handshakeKey = se::rng::random<size_t>();
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				sharedImpl->handshakeResponseCallback = [&handshakeSuccess, &handshakeKey](const HandshakeUDP& receivedHandshake)
				{
					se_assert(receivedHandshake.isValid());
					if (receivedHandshake.key == handshakeKey)
					{
						handshakeSuccess = true;
					}
				};
			}
			const time::Time sendInterval = time::fromSeconds(1.0f / 30.0f);
			const time::Time beginTime = time::now();
			HandshakeUDP handshake;
			handshake.key = handshakeKey;
			BinaryWriter binaryWriter;
			binaryWriter.serial(handshake);
			startReceiving();
			while (!handshakeSuccess)
			{
				update();
				
				if (!sendPacket(binaryWriter, PacketType::handshake))
				{
					break;
				}

				if (time::now() - beginTime >= connectionTimeout)
				{
					break;
				}

				time::sleep(sendInterval);
			}

			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				sharedImpl->handshakeResponseCallback = std::function<void(const HandshakeUDP&)>();
			}

			if (handshakeSuccess)
			{
				DEBUG_LOG(1, "successfully connected to the remote endpoint at: " + remoteEndpoint.address.toString() + ":" + remoteEndpoint.port.toString());
				return true;
			}
			else
			{
				DEBUG_LOG(1, "failed to exchange valid handshakes with the remote endpoint at: " + remoteEndpoint.address.toString() + ":" + remoteEndpoint.port.toString());
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				sharedImpl->connectedEndpoint = boost::asio::ip::udp::endpoint();
				return false;
			}
		}

		void SocketUDP::disconnect()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			boost::system::error_code error;
			sharedImpl->socket.shutdown(boost::asio::socket_base::shutdown_both, error);
			if (error)
			{
				DEBUG_LOG(1, "failed to shutdown. Boost asio error: " + error.message());
			}
			sharedImpl->socket.close(error);
			if (error)
			{
				DEBUG_LOG(1, "failed to close. Boost asio error: " + error.message());
			}
			sharedImpl->connectedEndpoint = boost::asio::ip::udp::endpoint();
		}
		
		void SocketUDP::clearReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
			sharedImpl->receivedPackets.clear();
		}

		bool SocketUDP::sendPacket(const BinaryWriter& binaryWriter, const PacketType packetType)
		{
			if (isConnected())
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sendPacket(binaryWriter, sharedImpl->connectedEndpoint, packetType);
			}
			else
			{
				se_assert(false && "connect() hasn't been called.");
				return false;
			}
		}

		bool SocketUDP::sendPacket(const BinaryWriter& binaryWriter, const boost::asio::ip::udp::endpoint& endpoint, const PacketType packetType)
		{
			return sharedImpl->sendPacket(binaryWriter, endpoint, packetType);
		}

		bool SocketUDP::resizeReceiveBuffer(const size_t newSize)
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			if (sharedImpl->receiving)
				return false;
			sharedImpl->receiveBuffer.resize(newSize);
			return true;
		}

		bool SocketUDP::startReceiving()
		{
			if (!isOpen())
			{
				open();
			}

			if (isReceiving())
			{
				return true;
			}

			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			sharedImpl->lastReceiveTime = time::now();
			sharedImpl->receiving = true;
			clearReceivedPackets();
			resumeReceiving();

			DEBUG_LOG(1, "successfully started receiving.");

			return true;
		}

		void SocketUDP::setOnReceiveCallback(const std::function<void(BinaryReader&, const boost::asio::ip::udp::endpoint&)> onReceiveCallback)
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			sharedImpl->onReceiveCallback = onReceiveCallback;
		}

		void SocketUDP::resumeReceiving()
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			se_assert(sharedImpl->receiving);
			try
			{
				sharedImpl->socket.async_receive_from(boost::asio::buffer(sharedImpl->receiveBuffer), sharedImpl->senderEndpoint,
					boost::bind(&SocketUDP::SharedImpl::receiveHandler, sharedImpl->shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			}
			catch (const std::exception& e)
			{
				log::error(std::string("Exception thrown: ") + e.what());
			}
		}

		void SocketUDP::update()
		{
			//Received packets
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);

			//Heartbeat
			if (isConnected() && time::now() - sharedImpl->lastSendTime > sharedImpl->heartbeatInterval)
			{
				sendPacket(BinaryWriter(), PacketType::heartbeat);
			}

			std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
			if (sharedImpl->onReceiveCallback)
			{
				for (size_t i = 0; i < sharedImpl->receivedPackets.size(); i++)
				{
					BinaryReader binaryReader(sharedImpl->receivedPackets[i]->buffer.data(), sharedImpl->receivedPackets[i]->buffer.size());
					sharedImpl->onReceiveCallback(binaryReader, sharedImpl->receivedPackets[i]->senderEndpoint);
				}
			}
			clearReceivedPackets();
		}

		void SocketUDP::setHeartbeatInterval(const time::Time interval)
		{
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			sharedImpl->heartbeatInterval = interval;
		}

		time::Time SocketUDP::getHeartbeatInterval() const
		{
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			return sharedImpl->heartbeatInterval;
		}

		bool SocketUDP::isReceiving() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->receiving;
		}

		bool SocketUDP::isOpen() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->socket.is_open();
		}

		Port SocketUDP::getLocalPort() const
		{
			return sharedImpl->getLocalPort();
		}

		bool SocketUDP::isConnected() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->connectedEndpoint != boost::asio::ip::udp::endpoint();
		}

		boost::asio::ip::udp::endpoint SocketUDP::getLocalEndpoint() const
		{
			return sharedImpl->getLocalEndpoint();
		}

		boost::asio::ip::udp::endpoint SocketUDP::getConnectedEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->connectedEndpoint;
		}

		void SocketUDP::setReuseAddress(const bool enabled)
		{
			se_assert(isOpen());
			boost::system::error_code error;
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			sharedImpl->socket.set_option(boost::asio::socket_base::reuse_address(enabled), error);
			if (error)
			{
				DEBUG_LOG(1, "failed to set reuse_address option. Boost asio error: " + error.message());
			}
		}

		bool SocketUDP::getReuseAddress() const
		{
			boost::asio::socket_base::reuse_address option;
			boost::system::error_code error;
			std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
			sharedImpl->socket.get_option(option, error);
			if (error)
			{
				DEBUG_LOG(1, "failed to get reuse_address option. Boost asio error: " + error.message());
			}
			return option;
		}
		
		size_t SocketUDP::getSentBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->sentBytes;
		}

		size_t SocketUDP::getReceivedBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
			return sharedImpl->receivedBytes;
		}

		void SocketUDP::setDebugLogLevel(const int level)
		{
			sharedImpl->setDebugLogLevel(level);
		}

		int SocketUDP::getDebugLogLevel() const
		{
			return sharedImpl->getDebugLogLevel();
		}
		
		SocketUDP::SharedImpl::SharedImpl(IOService& _ioService)
			: ioService(_ioService)
			, socket(getImplementationRef(_ioService))
			, receiveBuffer(65536)
		{

		}

		bool SocketUDP::SharedImpl::sendPacket(const BinaryWriter& binaryWriter, const boost::asio::ip::udp::endpoint& endpoint, const PacketType packetType)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;

			const SizeType packetSize = SizeType(binaryWriter.getSize());
			std::vector<boost::asio::const_buffer> buffers;
			buffers.push_back(boost::asio::buffer(&packetType, sizeof(packetType)));
			buffers.push_back(boost::asio::buffer(&packetSize, sizeof(packetSize)));
			if (binaryWriter.getSize() > 0)
			{
				buffers.push_back(boost::asio::buffer(binaryWriter.getData(), binaryWriter.getSize()));
			}

			const size_t writtenBytes = socket.send_to(buffers, endpoint, 0, error);
			sentBytes += writtenBytes;
			if (error)
			{
				DEBUG_LOG(1, "send failed. Failed to send data buffer. Boost error: " + error.message());
				return false;
			}
			if (writtenBytes != boost::asio::buffer_size(buffers))
			{
				DEBUG_LOG(1, "send failed. Failed to send the whole packet.");
				return false;
			}

			DEBUG_LOG(2, "packet sent to: " + endpoint.address().to_string() + ":" + std::to_string(endpoint.port()) + ". Size: " + std::to_string(binaryWriter.getSize()));

			lastSendTime = time::now();
			return true;
		}

		void SocketUDP::SharedImpl::receiveHandler(const boost::system::error_code& error, std::size_t bytes)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);

			if (!error)
			{
				DEBUG_LOG(2, "received " + std::to_string(bytes) + " bytes from: " + senderEndpoint.address().to_string() + ":" + std::to_string(senderEndpoint.port()));
				if (debugLogLevel >= 3)
				{
					for (size_t i = 0; i < bytes; i++)
					{
						log::info("    [" + std::to_string(i) + "] " + toHexString(receiveBuffer[i]));
					}
				}
			}

			se_assert(socketUDP);
			se_assert(receiving);
			lastReceiveTime = time::now();
			receivedBytes += bytes;
			
			if (error)
			{
				if (error == boost::asio::error::operation_aborted)
				{
					DEBUG_LOG(1, "failed to receive. Boost asio error: operation_aborted");
				}
				else if (error == boost::asio::error::connection_refused)
				{
					DEBUG_LOG(1, "failed to receive. Boost asio error: connection_refused. Did you choose the wrong port?");
				}
				else if (error == boost::asio::error::eof)
				{
					DEBUG_LOG(1, "remote socket closed connection.");
				}
				else if (error == boost::asio::error::connection_reset)
				{
					DEBUG_LOG(1, "remote socket closed connection.");
				}
				else
				{
					DEBUG_LOG(1, "error while receiving. Ignored boost asio error: " + std::to_string(error.value()) + ": " + error.message());
				}
			}
			
			if (bytes > 0)
			{
				BinaryReader binaryReader(&receiveBuffer[0], bytes);

				//Read header
				PacketType packetType;
				if (binaryReader.serial(packetType))
				{
					SizeType packetSize;
					if (binaryReader.serial(packetSize))
					{
						if (packetSize == binaryReader.getBytesRemaining())
						{
							if (packetType == PacketType::undefined)
							{
								if (onReceiveCallback && binaryReader.getBytesRemaining())
								{
									std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
									receivedPackets.push_back(std::unique_ptr<ReceivedPacket>());
									receivedPackets.back().reset(new ReceivedPacket());
									receivedPackets.back()->buffer.resize(binaryReader.getBytesRemaining());
									memcpy(receivedPackets.back()->buffer.data(), binaryReader.getData() + binaryReader.getOffset(), binaryReader.getBytesRemaining());
									receivedPackets.back()->senderEndpoint = senderEndpoint;
								}
							}
							else if (packetType == PacketType::heartbeat)
							{
								se_assert(binaryReader.getBytesRemaining() == 0);
							}
							else if (packetType == PacketType::handshake)
							{
								HandshakeUDP handshake;
								if (binaryReader.serial(handshake))
								{
									HandshakeUDP responseHandshake;
									responseHandshake.key = handshake.key;
									BinaryWriter binaryWriter;
									binaryWriter.serial(responseHandshake);
									sendPacket(binaryWriter, senderEndpoint, PacketType::handshakeResponse);
								}
								else
								{
									log::warning("Received packet containing invalid data.");
								}
							}
							else if (packetType == PacketType::handshakeResponse)
							{
								HandshakeUDP handshake;
								if (binaryReader.serial(handshake))
								{
									if (handshakeResponseCallback)
									{
										handshakeResponseCallback(handshake);
									}
								}
								else
								{
									log::warning("Received packet containing invalid data.");
								}
							}
							else
							{
								log::warning("Received packet of unknown type: " + std::to_string(int(packetType)));
							}
						}
						else
						{
							log::warning("Remaining packet size was different from that of the declared packet size.");
						}
					}
				}
			}

			socketUDP->resumeReceiving();
		}

		boost::asio::ip::udp::endpoint SocketUDP::SharedImpl::getLocalEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;
			boost::asio::ip::udp::endpoint localEndpoint = socket.local_endpoint(error);
			if (error)
			{
				if (debugLogLevel >= 1)
				{
					log::info("SocketUDP: failed to get local endpoint. Boost asio error: " + error.message());
				}
			}
			return localEndpoint;
		}

		Port SocketUDP::SharedImpl::getLocalPort() const
		{
			return Port(getLocalEndpoint().port());
		}

		void SocketUDP::SharedImpl::setDebugLogLevel(const int level)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			debugLogLevel = level;
		}

		int SocketUDP::SharedImpl::getDebugLogLevel() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return debugLogLevel;
		}
	}
}
