#include "stdafx.h"
#include "SpehsEngine/Net/SocketUDP.h"

#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/IOServiceUtilityFunctions.h"
#include "SpehsEngine/Net/ISocket.h"
#include "SpehsEngine/Net/Port.h"
#include "SpehsEngine/Net/Address.h"
#include "SpehsEngine/Net/PacketMessage.h"
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <thread>
#include <mutex>

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info("SocketUDP(" + getLocalPort().toString() + "): " + message); \
}


namespace se
{
	namespace
	{
		typedef uint16_t SizeType;

#ifdef SE_FINAL_RELEASE
		static const se::Time connectionTimeout = Time::fromSeconds(5.0f);
#else
		static const se::Time connectionTimeout = Time::fromSeconds(10000.0f);
#endif
}

	namespace net
	{
		inline boost::asio::ip::udp::endpoint toAsioEndpoint(const Endpoint& endpoint)
		{
			switch (endpoint.address.type)
			{
			case Address::Type::None: break;
			case Address::Type::Ipv4: return boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4(endpoint.address.data0), endpoint.port.value);
			case Address::Type::Ipv6:
			{
				boost::asio::ip::address_v6::bytes_type bytes;
				memcpy((void*)bytes[0], &endpoint.address.data0, 16);
				return boost::asio::ip::udp::endpoint(boost::asio::ip::address_v6(bytes), endpoint.port.value);
			}
			}
			return boost::asio::ip::udp::endpoint();
		}

		inline Endpoint fromAsioEndpoint(const boost::asio::ip::udp::endpoint& asioEndpoint)
		{
			Endpoint endpoint;
			if (asioEndpoint.address().is_v4())
			{
				endpoint.address = Address(asioEndpoint.address().to_v4().to_uint());
			}
			else if (asioEndpoint.address().is_v6())
			{
				const boost::asio::ip::address_v6::bytes_type bytes = asioEndpoint.address().to_v6().to_bytes();
				endpoint.address = Address(bytes[0]);
			}
			else
			{
				return Endpoint();
			}
			endpoint.port = Port(asioEndpoint.port());
			return endpoint;
		}

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
		struct SocketUDP : public ISocketUDP
		{
			//Received packets
			struct SharedImpl : public boost::enable_shared_from_this<SharedImpl>
			{
				struct ReceivedPacket
				{
					std::vector<uint8_t> buffer;
					Endpoint senderEndpoint;
				};

				SharedImpl(IOService& _ioService)
					: ioService(_ioService)
					, socket(getImplementationRef(_ioService))
					, receiveBuffer(65536)
				{
				}

				bool sendPacket(const BinaryWriter& binaryWriter, const boost::asio::ip::udp::endpoint& endpoint, const PacketType packetType)
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

					DEBUG_LOG(2, "packet sent to: " + fromAsioEndpoint(endpoint).toString() + ". Size: " + std::to_string(binaryWriter.getSize()));

					lastSendTime = getEpochTime();
					return true;
				}

				void receiveHandler(const boost::system::error_code& error, std::size_t bytes)
				{
					std::lock_guard<std::recursive_mutex> lock1(mutex);

					if (!error)
					{
						DEBUG_LOG(2, "received " + std::to_string(bytes) + " bytes from: " + fromAsioEndpoint(senderEndpoint).toString());
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
					lastReceiveTime = getEpochTime();
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
											receivedPackets.back()->senderEndpoint = fromAsioEndpoint(senderEndpoint);
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

				Endpoint getLocalEndpoint() const
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
					return fromAsioEndpoint(localEndpoint);
				}

				Port getLocalPort() const
				{
					return getLocalEndpoint().port;
				}

				void setDebugLogLevel(const int level)
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					debugLogLevel = level;
				}

				int getDebugLogLevel() const
				{
					std::lock_guard<std::recursive_mutex> lock(mutex);
					return debugLogLevel;
				}

				mutable std::recursive_mutex mutex;
				Endpoint connectedEndpoint;
				boost::asio::ip::udp::endpoint senderEndpoint;//Used by the receiver thread. Think carefully about thread sync!
				IOService& ioService;
				boost::asio::ip::udp::socket socket;
				std::vector<unsigned char> receiveBuffer;
				Time lastReceiveTime;
				Time lastSendTime;
				Time heartbeatInterval = Time::fromSeconds(5.0f);
				bool receiving = false;
				std::function<void(BinaryReader&, const Endpoint&)> onReceiveCallback;//User defined receive handler
				std::function<void(const HandshakeUDP&)> handshakeResponseCallback;
				std::recursive_mutex receivedPacketsMutex;
				std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;
				SocketUDP* socketUDP = nullptr;
				size_t sentBytes = 0;
				size_t receivedBytes = 0;

				/*
				level 1: prints most essential state changes.
				level 2: prints some network traffic numbers.
				level 3: prints receive buffer in hex string.
				*/
				int debugLogLevel = 0;
			};

			SocketUDP(IOService& _ioService)
				: sharedImpl(new SharedImpl(_ioService))
			{
				sharedImpl->socketUDP = this;
			}

			~SocketUDP()
			{
				close();
				std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
				se_assert(sharedImpl->socketUDP);
				sharedImpl->socketUDP = nullptr;
			}

			bool open() final
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

			void close() final
			{
				{
					std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
					if (sharedImpl->socket.is_open())
					{
						sharedImpl->onReceiveCallback = std::function<void(BinaryReader&, const Endpoint&)>();
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

			bool bind(const Port& port) final
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

			bool connect(const Endpoint& remoteEndpoint) final
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
							sharedImpl->connectedEndpoint = Endpoint(Address(remoteAsioEndpoint.address().to_v4().to_uint()), Port(remoteAsioEndpoint.port()));
						}
					}
				}

				/* Send out a keyed handshake and wait for a response. */
				bool handshakeSuccess = false;
				const size_t handshakeKey = rng::random<size_t>();
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
				const Time sendInterval = Time::fromSeconds(1.0f / 30.0f);
				const Time beginTime = getEpochTime();
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

					if (getEpochTime() - beginTime >= connectionTimeout)
					{
						break;
					}

					sleep(sendInterval);
				}

				{
					std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
					sharedImpl->handshakeResponseCallback = std::function<void(const HandshakeUDP&)>();
				}

				if (handshakeSuccess)
				{
					DEBUG_LOG(1, "successfully connected to the remote endpoint at: " + remoteEndpoint.toString());
					return true;
				}
				else
				{
					DEBUG_LOG(1, "failed to exchange valid handshakes with the remote endpoint at: " + remoteEndpoint.toString());
					std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
					sharedImpl->connectedEndpoint = Endpoint();
					return false;
				}
			}

			void disconnect() final
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
				sharedImpl->connectedEndpoint = Endpoint();
			}

			void clearReceivedPackets()
			{
				std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
				std::lock_guard<std::recursive_mutex> lock2(sharedImpl->receivedPacketsMutex);
				sharedImpl->receivedPackets.clear();
			}

			bool sendPacket(const BinaryWriter& binaryWriter, const PacketType packetType) final
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

			bool sendPacket(const BinaryWriter& binaryWriter, const Endpoint& endpoint, const PacketType packetType) final
			{
				return sharedImpl->sendPacket(binaryWriter, toAsioEndpoint(endpoint), packetType);
			}

			bool resizeReceiveBuffer(const size_t newSize) final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				if (sharedImpl->receiving)
					return false;
				sharedImpl->receiveBuffer.resize(newSize);
				return true;
			}

			bool startReceiving() final
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
				sharedImpl->lastReceiveTime = getEpochTime();
				sharedImpl->receiving = true;
				clearReceivedPackets();
				resumeReceiving();

				DEBUG_LOG(1, "successfully started receiving.");

				return true;
			}

			void setOnReceiveCallback(const std::function<void(BinaryReader&, const Endpoint&)> onReceiveCallback) final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				sharedImpl->onReceiveCallback = onReceiveCallback;
			}

			void resumeReceiving()
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

			void update() final
			{
				//Received packets
				std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);

				//Heartbeat
				if (isConnected() && getEpochTime() - sharedImpl->lastSendTime > sharedImpl->heartbeatInterval)
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

			void setHeartbeatInterval(const Time interval) final
			{
				std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
				sharedImpl->heartbeatInterval = interval;
			}

			Time getHeartbeatInterval() const final
			{
				std::lock_guard<std::recursive_mutex> lock1(sharedImpl->mutex);
				return sharedImpl->heartbeatInterval;
			}

			bool isReceiving() const final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sharedImpl->receiving;
			}

			bool isOpen() const final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sharedImpl->socket.is_open();
			}

			Port getLocalPort() const final
			{
				return sharedImpl->getLocalPort();
			}

			bool isConnected() const final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sharedImpl->connectedEndpoint.isValid();
			}

			Endpoint getLocalEndpoint() const final
			{
				return sharedImpl->getLocalEndpoint();
			}

			Endpoint getConnectedEndpoint() const final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sharedImpl->connectedEndpoint;
			}

			void setReuseAddress(const bool enabled) final
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

			bool getReuseAddress() const final
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

			size_t getSentBytes() const final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sharedImpl->sentBytes;
			}

			size_t getReceivedBytes() const final
			{
				std::lock_guard<std::recursive_mutex> lock(sharedImpl->mutex);
				return sharedImpl->receivedBytes;
			}

			void setDebugLogLevel(const int level) final
			{
				sharedImpl->setDebugLogLevel(level);
			}

			int getDebugLogLevel() const final
			{
				return sharedImpl->getDebugLogLevel();
			}

			boost::shared_ptr<SharedImpl> sharedImpl;
		};
	}
}
