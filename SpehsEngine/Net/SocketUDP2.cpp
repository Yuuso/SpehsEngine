#include "stdafx.h"
#include "SpehsEngine/Net/SocketUDP2.h"

#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "boost/bind.hpp"
#include "boost/asio/placeholders.hpp"
#include <string>

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
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
		SocketUDP2::SocketUDP2(IOService& _ioService, const std::string& _debugName)
			: debugName(_debugName)
			, ioService(_ioService)
			, boostSocket(_ioService.getImplementationRef())
			, receiveBuffer1(65536)
			, receiveBuffer2(65536)
		{

		}

		void SocketUDP2::update()
		{
			/*
				Copy all currently received packets into local non-mutex protected memory.
					-receiveHandler may change in-between packet processing
					-new packets may arrive during processing
			*/
			std::vector<std::unique_ptr<ReceivedPacketSocketUDP2>> receivedPacketsToProcess;
			{
				std::lock_guard<std::recursive_mutex> lock1(mutex);
				std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
				receivedPacketsToProcess.swap(receivedPackets);
			}
			if (getDebugLogLevel() >= 1 && receivedPacketsToProcess.size() > 1000)
			{
				se::log::warning("SocketUDP2: processing " + std::to_string(receivedPacketsToProcess.size()) + " packets in a single update.");
			}

			size_t packetIndex = 0u;
			std::function<void(ReceivedPacketSocketUDP2&)> handler;
			while (packetIndex < receivedPacketsToProcess.size())
			{
				// Update handler
				{
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					handler = receiveHandler;
				}
				handler(*receivedPacketsToProcess[packetIndex++]);
			}
		}

		bool SocketUDP2::open()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (boostSocket.is_open())
			{
				return true;
			}
			boost::system::error_code error;
			boostSocket.open(boost::asio::ip::udp::v4(), error);
			if (error)
			{
				DEBUG_LOG(1, "failed to open. " + error.message());
				return false;
			}
			return true;
		}

		void SocketUDP2::close()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (boostSocket.is_open())
			{
				boost::system::error_code error;
				boostSocket.shutdown(boost::asio::socket_base::shutdown_both, error);
				if (error)
				{
					DEBUG_LOG(1, "failed to shutdown. " + error.message());
				}
				boostSocket.close(error);
				if (error)
				{
					DEBUG_LOG(1, "failed to close. " + error.message());
				}
				else
				{
					DEBUG_LOG(1, "socket closed.");
				}
			}
		}

		bool SocketUDP2::bind(const Port port)
		{
			if (!isOpen())
			{
				if (!open())
				{
					if (debugLogLevel >= 1)
					{
						DEBUG_LOG(1, "failed to bind. Socket was not open and could not be opened.");
					}
					return false;
				}
				return false;
			}
			boost::system::error_code error;

			const boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), port.value);
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boostSocket.bind(endpoint, error);
			if (error)
			{
				DEBUG_LOG(1, "failed to bind. " + error.message());
				return false;
			}
			else
			{
				DEBUG_LOG(1, "binding successful.");
				debugLocalPort = std::to_string(port.value);

				return true;
			}
		}

		void SocketUDP2::sendPacket(const WriteBuffer& writeBuffer, const boost::asio::ip::udp::endpoint& endpoint)
		{
			const std::vector<boost::asio::const_buffer> buffers{ boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()) };
			return sendPacket(buffers, endpoint);
		}

		void SocketUDP2::sendPacket(const std::vector<boost::asio::const_buffer>& buffers, const boost::asio::ip::udp::endpoint& endpoint)
		{
			const size_t bufferSize = boost::asio::buffer_size(buffers);
			if (bufferSize == 0)
			{
				DEBUG_LOG(1, "empty buffer provided. Returning success.");
				return;
			}

			std::lock_guard<std::recursive_mutex> lock(mutex);
			boostSocket.async_send_to(buffers, endpoint,
				boost::bind(&SocketUDP2::boostSendHandler, shared_from_this(), endpoint,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		void SocketUDP2::boostSendHandler(const boost::asio::ip::udp::endpoint endpoint, const boost::system::error_code& error, const std::size_t bytes)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			if (error)
			{
				if (error.value() == 10040)
				{
					/*
						"A message sent on a datagram socket was larger than the internal message buffer or some other network limit,
						or the buffer used to receive a datagram into was smaller than the datagram itself"
					*/
					se::log::warning(error.message());
				}
				else
				{
					DEBUG_LOG(1, "send failed. Failed to send data buffer. Boost error: " + error.message());
				}
				return;
			}
			else
			{
				sentBytes += bytes;
				lastSendTime = time::now();
				DEBUG_LOG(2, "packet sent to: " + endpoint.address().to_string() + ":" + std::to_string(endpoint.port()) + ". Size: " + std::to_string(bytes));
			}
		}

		void SocketUDP2::setReceiveHandler(const std::function<void(ReceivedPacketSocketUDP2&)>& _receiveHandler)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			receiveHandler = _receiveHandler;
		}
		
		bool SocketUDP2::startReceiving()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			se_assert(!receiving);
			try
			{
				receiving = true;
				boostSenderEndpoint = boost::asio::ip::udp::endpoint();
				boostSocket.async_receive_from(boost::asio::buffer(receiveBuffer1), boostSenderEndpoint,
					boost::bind(&SocketUDP2::boostReceiveHandler, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				return true;
			}
			catch (const std::exception& e)
			{
				receiving = false;
				log::error(std::string("SocketUDP2::startReceiving(): async_receive_from(): exception thrown: ") + e.what());
				return false;
			}
		}

		void SocketUDP2::boostReceiveHandler(const boost::system::error_code& error, const std::size_t bytes)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			se_assert(receiving);
			receiving = false;

			// Swap the receive buffers and start receiving asap
			boost::asio::ip::udp::endpoint boostSenderEndpoint2 = boostSenderEndpoint;
			receiveBuffer1.swap(receiveBuffer2);
			startReceiving();

			DEBUG_LOG(2, "received " + std::to_string(bytes) + " bytes from: " + boostSenderEndpoint2.address().to_string() + ":" + std::to_string(boostSenderEndpoint2.port()));
			if (bytes > 0 && debugLogLevel >= 3)
			{
				for (size_t i = 0; i < bytes; i++)
				{
					log::info("    [" + std::to_string(i) + "] " + toHexString(receiveBuffer2[i]));
				}
			}

			lastReceiveTime = time::now();
			receivedBytes += bytes;

			ReceivedPacketSocketUDP2::ErrorType errorType = ReceivedPacketSocketUDP2::ErrorType::None;
			if (error)
			{
				if (error == boost::asio::error::operation_aborted)
				{
					DEBUG_LOG(1, "error while receiving: operation_aborted");
				}
				else if (error == boost::asio::error::connection_refused)
				{
					DEBUG_LOG(1, "error while receiving: connection_refused. Did you choose the wrong port? " + error.message());
					errorType = ReceivedPacketSocketUDP2::ErrorType::ConnectionRefused;
				}
				else if (error == boost::asio::error::eof)
				{
					DEBUG_LOG(1, "error while receiving: eof. Remote socket closed connection.");
				}
				else if (error == boost::asio::error::connection_reset)
				{
					DEBUG_LOG(1, "error while receiving: connection_reset. Remote socket closed connection.");
				}
				else if (error == boost::asio::error::bad_descriptor)
				{
					DEBUG_LOG(1, "error while receiving: bad descriptor.");
				}
				else
				{
					DEBUG_LOG(1, "error while receiving. " + std::to_string(error.value()) + ": " + error.message());
				}
			}

			if (bytes > 0 || errorType != ReceivedPacketSocketUDP2::ErrorType::None)
			{
				std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
				receivedPackets.push_back(std::make_unique<ReceivedPacketSocketUDP2>());
				receivedPackets.back()->senderEndpoint = boostSenderEndpoint2;
				receivedPackets.back()->buffer.resize(bytes);
				receivedPackets.back()->errorType = errorType;
				memcpy(receivedPackets.back()->buffer.data(), receiveBuffer2.data(), bytes);
			}
		}

		boost::asio::ip::udp::endpoint SocketUDP2::resolveRemoteEndpoint(const Endpoint& remoteEndpoint) const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::asio::ip::udp::resolver resolverUDP(ioService.getImplementationRef());
			const boost::asio::ip::udp::resolver::query queryUDP(boost::asio::ip::udp::v4(), remoteEndpoint.address.toString(), remoteEndpoint.port.toString());
			boost::system::error_code error;
			boost::asio::ip::udp::resolver::iterator it = resolverUDP.resolve(queryUDP, error);
			if (error)
			{
				DEBUG_LOG(1, "failed to resolve remote endpoint. Boost error: " + error.message());
				return boost::asio::ip::udp::endpoint();
			}
			const boost::asio::ip::udp::resolver::iterator end;
			if (it != end)
			{
				return *it;
			}
			else
			{
				DEBUG_LOG(1, "failed to resolve remote endpoint.");
				return boost::asio::ip::udp::endpoint();
			}
		}

		bool SocketUDP2::isOpen() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return boostSocket.is_open();
		}

		bool SocketUDP2::isBound() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;
			boostSocket.local_endpoint(error);
			return !error;
		}

		bool SocketUDP2::isReceiving() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return receiving;
		}

		Port SocketUDP2::getLocalPort() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;
			boost::asio::ip::udp::endpoint localEndpoint = boostSocket.local_endpoint(error);
			if (error)
			{
				if (debugLogLevel >= 1)
				{
					log::info(debugName + ": failed to get local endpoint. " + error.message());
				}
			}
			return localEndpoint.port();
		}

		uint64_t SocketUDP2::getSentBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return sentBytes;
		}

		uint64_t SocketUDP2::getReceivedBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return receivedBytes;
		}

		uint16_t SocketUDP2::getMaxSendBufferSize() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::asio::socket_base::send_buffer_size option;
			boost::system::error_code error;
			boostSocket.get_option(option, error);
			if (error)
			{
				se_assert(false && "SocketUDP2::getMaxSendBufferSize() failed");
				return 0;
			}
			else
			{
				const uint16_t size = uint16_t(option.value());
				return size;
			}
		}

		void SocketUDP2::setDebugLogLevel(const int level)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			debugLogLevel = level;
		}

		int SocketUDP2::getDebugLogLevel() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return debugLogLevel;
		}
	}
}
