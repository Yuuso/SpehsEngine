#pragma once

#include "boost/asio/ip/udp.hpp"
#include "boost/system/error_code.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "SpehsEngine/Core/Serial/BinaryWriter.h"
#include "SpehsEngine/Net/Endpoint.h"


namespace se
{
	namespace net
	{
		class IOService;

		struct ReceivedPacketSocketUDP2
		{
			enum class ErrorType { None, ConnectionRefused };
			std::vector<uint8_t> buffer;
			boost::asio::ip::udp::endpoint senderEndpoint;
			ErrorType errorType = ErrorType::None;
		};

		/*
			Wraps minimal UDP socket usage.
		*/
		class SocketUDP2 : public boost::enable_shared_from_this<SocketUDP2>
		{
		private:
			void pushToBuffers(std::vector<boost::asio::const_buffer>& buffers, const BinaryWriter& binaryWriter)
			{
				buffers.push_back(boost::asio::const_buffer(binaryWriter.getData(), binaryWriter.getSize()));
			}
			template<typename ... BinaryWriters>
			void pushToBuffers(std::vector<boost::asio::const_buffer>& buffers, const BinaryWriter& binaryWriter, const BinaryWriters&... binaryWriters)
			{
				pushToBuffers(buffers, binaryWriter, binaryWriters...);
			}
		public:

			SocketUDP2(IOService& ioService, const std::string& debugName = "SocketUDP2");

			/*
				Calls the currently set receive handler for all arrived packets.
				Packets are cleared if there is no receive handler, or if the receive handler callsite resets the receive handler.
			*/
			void update();

			bool open();
			void close();
			bool bind(const Port port = Port());
			bool startReceiving();
			void setReceiveHandler(const std::function<void(ReceivedPacketSocketUDP2&)>& _receiveHandler);
			void sendPacket(const BinaryWriter& binaryWriter, const boost::asio::ip::udp::endpoint& endpoint);
			void sendPacket(const std::vector<boost::asio::const_buffer>& buffers, const boost::asio::ip::udp::endpoint& endpoint);
			template<typename ... BinaryWriters>
			void sendPacket(const BinaryWriters&... binaryWriters, const boost::asio::ip::udp::endpoint& endpoint)
			{
				std::vector<boost::asio::const_buffer> buffers;
				pushToBuffers(buffers, binaryWriters...);
				sendPacket(buffers, endpoint);
			}

			bool isOpen() const;
			bool isBound() const;
			bool isReceiving() const;
			Port getLocalPort() const;
			boost::asio::ip::udp::endpoint resolveRemoteEndpoint(const Endpoint& endpoint) const;
			uint64_t getSentBytes() const;
			uint64_t getReceivedBytes() const;
			uint16_t getMaxSendBufferSize() const;
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

			const std::string debugName;

		private:

			void boostSendHandler(const boost::asio::ip::udp::endpoint endpoint, const boost::system::error_code& error, const std::size_t bytes);
			void boostReceiveHandler(const boost::system::error_code& error, const std::size_t bytes);

			mutable std::recursive_mutex mutex;
			IOService& ioService;
			boost::asio::ip::udp::endpoint boostSenderEndpoint; // Used to receive the sender endpoint from boost during async receive.
			boost::asio::ip::udp::socket boostSocket;
			std::vector<unsigned char> receiveBuffer1;
			std::vector<unsigned char> receiveBuffer2;
			bool receiving = false;
			std::function<void(ReceivedPacketSocketUDP2&)> receiveHandler;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			uint64_t sentBytes = 0;
			uint64_t receivedBytes = 0;
			std::recursive_mutex receivedPacketsMutex;
			std::vector<std::unique_ptr<ReceivedPacketSocketUDP2>> receivedPackets;
			int debugLogLevel = 0;
			std::string debugLocalPort; // Only set when bound to new port, never unset.
		};
	}
}
