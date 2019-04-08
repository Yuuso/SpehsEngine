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
		public:
			
			struct ReceivedPacket
			{
				std::vector<uint8_t> buffer;
				boost::asio::ip::udp::endpoint senderEndpoint;
			};

			SocketUDP2(IOService& ioService);

			/*
				Calls the currently set receive handler for all arrived packets.
				Packets are cleared if there is no receive handler, or if the receive handler callsite resets the receive handler.
			*/
			void update();

			bool open();
			void close();
			bool bind(const Port& port);
			bool sendPacket(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);
			void startReceiving();
			void setReceiveHandler(const std::function<void(std::vector<uint8_t>&, const boost::asio::ip::udp::endpoint&)>& _receiveHandler);
			boost::asio::ip::udp::endpoint resolveRemoteEndpoint(const Endpoint& endpoint);

			bool isOpen() const;
			bool isReceiving() const;
			boost::asio::ip::udp::endpoint getLocalEndpoint() const;
			Port getLocalPort() const;
			size_t getSentBytes() const;
			size_t getReceivedBytes() const;
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

		private:

			void boostReceiveHandler(const boost::system::error_code& error, const std::size_t bytes);

			mutable std::recursive_mutex mutex;
			IOService& ioService;
			boost::asio::ip::udp::endpoint senderEndpoint;
			boost::asio::ip::udp::socket boostSocket;
			std::vector<unsigned char> receiveBuffer;
			bool receiving = false;
			std::function<void(std::vector<uint8_t>&, const boost::asio::ip::udp::endpoint&)> receiveHandler;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			size_t sentBytes = 0;
			size_t receivedBytes = 0;
			int debugLogLevel = 0;
			std::recursive_mutex receivedPacketsMutex;
			std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;
		};
	}
}
