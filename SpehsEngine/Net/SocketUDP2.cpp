#include "stdafx.h"
#include "SpehsEngine/Net/SocketUDP2.h"

#include <string>
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "boost/bind.hpp"
#include "boost/asio/placeholders.hpp"

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
}

/**///#pragma optimize("", off)

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
			, receiveBuffer(65536)
		{

		}

		void SocketUDP2::update()
		{
			/*
				Receive handler calls are made outside the mutex protection:
					-receiveHandler may change in-between packet processing
					-new packets may arrive during processing
			*/
			size_t packetIndex = 0u;
			std::vector<uint8_t> data;
			boost::asio::ip::udp::endpoint senderEndpoint;
			std::function<void(std::vector<uint8_t>&, const boost::asio::ip::udp::endpoint&)> handler;
			while (true)
			{
				{
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
					if (packetIndex < receivedPackets.size())
					{
						//Copy contents to non-mutex protected memory
						data.swap(receivedPackets[packetIndex]->buffer);
						senderEndpoint = receivedPackets[packetIndex]->senderEndpoint;
						handler = receiveHandler;
						packetIndex++;
					}
					else
					{
						break;
					}
				}
				handler(data, senderEndpoint);
			}

			std::lock_guard<std::recursive_mutex> lock1(mutex);
			std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
			receivedPackets.clear();
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

		bool SocketUDP2::sendPacket(const WriteBuffer& writeBuffer, const boost::asio::ip::udp::endpoint& endpoint)
		{
			const std::vector<boost::asio::const_buffer> buffers{ boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()) };
			return sendPacket(buffers, endpoint);
		}

		bool SocketUDP2::sendPacket(const std::vector<boost::asio::const_buffer>& buffers, const boost::asio::ip::udp::endpoint& endpoint)
		{
			const size_t bufferSize = boost::asio::buffer_size(buffers);
			if (bufferSize == 0)
			{
				DEBUG_LOG(1, "empty buffer provided. Returning success.");
				return true;
			}

			std::lock_guard<std::recursive_mutex> lock(mutex);
			boost::system::error_code error;
			
			const size_t writtenBytes = boostSocket.send_to(buffers, endpoint, 0, error);
			sentBytes += writtenBytes;
			if (error)
			{
				DEBUG_LOG(1, "send failed. Failed to send data buffer. Boost error: " + error.message());
				return false;
			}
			if (writtenBytes != bufferSize)
			{
				DEBUG_LOG(1, "send failed. Failed to send the whole packet.");
				return false;
			}

			DEBUG_LOG(2, "packet sent to: " + endpoint.address().to_string() + ":" + std::to_string(endpoint.port()) + ". Size: " + std::to_string(bufferSize));

			lastSendTime = time::now();
			return true;
		}

		void SocketUDP2::setReceiveHandler(const std::function<void(std::vector<uint8_t>&, const boost::asio::ip::udp::endpoint&)>& _receiveHandler)
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
				boostSocket.async_receive_from(boost::asio::buffer(receiveBuffer), boostSenderEndpoint,
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

			DEBUG_LOG(2, "received " + std::to_string(bytes) + " bytes from: " + boostSenderEndpoint.address().to_string() + ":" + std::to_string(boostSenderEndpoint.port()));
			if (bytes > 0 && debugLogLevel >= 3)
			{
				for (size_t i = 0; i < bytes; i++)
				{
					log::info("    [" + std::to_string(i) + "] " + toHexString(receiveBuffer[i]));
				}
			}

			se_assert(receiving);
			receiving = false;
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
					DEBUG_LOG(1, "error while receiving. " + std::to_string(error.value()) + ": " + error.message());
				}
			}

			if (bytes > 0)
			{
				std::lock_guard<std::recursive_mutex> lock2(receivedPacketsMutex);
				receivedPackets.push_back(std::make_unique<ReceivedPacket>());
				receivedPackets.back()->senderEndpoint = boostSenderEndpoint;
				receivedPackets.back()->buffer.resize(bytes);
				memcpy(receivedPackets.back()->buffer.data(), receiveBuffer.data(), bytes);
			}

			startReceiving();
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

		size_t SocketUDP2::getSentBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return sentBytes;
		}

		size_t SocketUDP2::getReceivedBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return receivedBytes;
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
