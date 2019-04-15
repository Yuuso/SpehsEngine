#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include "boost/asio/ip/udp.hpp"
#include "boost/system/error_code.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/signals2.hpp"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Net/Endpoint.h"

namespace se
{
	namespace net
	{
		class IOService;

		/*
			Wraps minimal UDP socket usage.
		*/
		class SocketUDP2 : public boost::enable_shared_from_this<SocketUDP2>
		{
		private:
			void pushToBuffers(std::vector<boost::asio::const_buffer>& buffers, const WriteBuffer& writeBuffer)
			{
				buffers.push_back(boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()));
			}
			template<typename ... Buffers>
			void pushToBuffers(std::vector<boost::asio::const_buffer>& buffers, const WriteBuffer& writeBuffer, const Buffers&... writeBuffers)
			{
				pushToBuffers(buffers, writeBuffer, writeBuffers...);
			}
		public:
			
			struct ReceivedPacket
			{
				std::vector<uint8_t> buffer;
				boost::asio::ip::udp::endpoint senderEndpoint;
			};

			SocketUDP2(IOService& ioService, const std::string& debugName = "SocketUDP2");

			/*
				Calls the currently set receive handler for all arrived packets.
				Packets are cleared if there is no receive handler, or if the receive handler callsite resets the receive handler.
			*/
			void update();

			bool open();
			void close();
			bool bind(const Port& port);
			void startReceiving();
			void setReceiveHandler(const std::function<void(std::vector<uint8_t>&, const boost::asio::ip::udp::endpoint&)>& _receiveHandler);
			bool sendPacket(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);
			bool sendPacket(const std::vector<boost::asio::const_buffer>& buffers, const boost::asio::ip::udp::endpoint& endpoint);
			template<typename ... Buffers>
			bool sendPacket(const Buffers&... writeBuffers, const boost::asio::ip::udp::endpoint& endpoint)
			{
				std::vector<boost::asio::const_buffer> buffers;
				pushToBuffers(buffers, writeBuffers...);
				return sendPacket(buffers, endpoint);
			}

			bool isOpen() const;
			bool isReceiving() const;
			Port getLocalPort() const;
			boost::asio::ip::udp::endpoint resolveRemoteEndpoint(const Endpoint& endpoint) const;
			size_t getSentBytes() const;
			size_t getReceivedBytes() const;
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

			const std::string debugName;

		private:

			void boostReceiveHandler(const boost::system::error_code& error, const std::size_t bytes);

			mutable std::recursive_mutex mutex;
			IOService& ioService;
			boost::asio::ip::udp::endpoint boostSenderEndpoint; // Used to receive the sender endpoint from boost during async receive.
			boost::asio::ip::udp::socket boostSocket;
			std::vector<unsigned char> receiveBuffer;
			bool receiving = false;
			std::function<void(std::vector<uint8_t>&, const boost::asio::ip::udp::endpoint&)> receiveHandler;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			size_t sentBytes = 0;
			size_t receivedBytes = 0;
			std::recursive_mutex receivedPacketsMutex;
			std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;
			int debugLogLevel = 0;
			std::string debugLocalPort; // Only set when bound to new port, never unset.
		};
	}
}
