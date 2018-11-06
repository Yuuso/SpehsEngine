#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <type_traits>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

#include "SpehsEngine/Net/ISocket.h"
#include "SpehsEngine/Net/Port.h"
#include "SpehsEngine/Net/Address.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Core/Time.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		class SocketUDP : public ISocket
		{
		private:
			typedef uint32_t ExpectedBytesType;
			typedef uint32_t Id;

		public:

			SocketUDP(IOService& ioService);
			virtual ~SocketUDP();

			/* Opens the socket. Must be called before packets can be received or sent. */
			bool open();
			void close();

			/* Binds the socket to a local endpoint. */
			bool bind(const Port& port);

			/*
				Connects the socket to a remote endpoint. Blocking call.
				Socket must be opened before calling.
				UDP is a connectionless protocol, connection status cannot be maintained.
			*/
			bool connect(const Endpoint& remoteEndpoint) override;
			/* Clears the connected endpoint value. */
			void disconnect();

			/* Process arrived packets(onReceive callbacks). */
			void update() override;

			/* Sends buffer to the connected endpoint. */
			bool sendPacket(const WriteBuffer& buffer);

			/* Sends buffer to a specified endpoint. */
			bool sendPacket(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);

			/* Returns false if the memory allocation fails, or the socket is currently receiving data. */
			bool resizeReceiveBuffer(const size_t newSize);

			/* Starts receiving data from the connected endpoint. Non-blocking call. */
			bool startReceiving();
			bool isReceiving() const;
			
			/* Returns the total number of sent bytes. Does not account bytes from the IP implementation. */
			size_t getSentBytes() const override;

			/* Returns the total number of received bytes. Does not account bytes from the IP implementation. */
			size_t getReceivedBytes() const override;

			/* Received packets must be processed with a specified receive handler. While there exists no callback, all incoming packets are discarded. */
			void setOnReceiveCallback(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&)> onReceiveCallback = std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&)>());

			bool isOpen() const;
			Port getLocalPort() const;
			bool isConnected() const;
			boost::asio::ip::udp::endpoint getConnectedEndpoint() const;

		private:

			void resumeReceiving();
			void clearReceivedPackets();
			
			//Received packets
			struct SharedImpl : public boost::enable_shared_from_this<SharedImpl>
			{
				SharedImpl(IOService& ioService);
				void receiveHandler(const boost::system::error_code& error, std::size_t bytes);

				struct ReceivedPacket
				{
					std::vector<uint8_t> buffer;
					boost::asio::ip::udp::endpoint senderEndpoint;
				};
				enum class ReceiveType { none, connection, any };
				mutable std::recursive_mutex mutex;
				boost::asio::ip::udp::endpoint connectedEndpoint;
				boost::asio::ip::udp::endpoint senderEndpoint;//Used by the receiver thread. Think carefully about thread sync!
				IOService& ioService;
				boost::asio::ip::udp::socket socket;
				std::vector<unsigned char> receiveBuffer;
				time::Time lastReceiveTime;
				bool receiving = false;
				ReceiveType receiveType = ReceiveType::none;
				std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&)> onReceiveCallback;//User defined receive handler
				std::recursive_mutex receivedPacketsMutex;
				std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;
				SocketUDP* socketUDP = nullptr;
				size_t sentBytes = 0;
				size_t receivedBytes = 0;
			};
			boost::shared_ptr<SharedImpl> sharedImpl;

		private:
			friend struct SharedImpl;
		};
	}
}
