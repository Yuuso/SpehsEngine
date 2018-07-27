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

#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Core/Time.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		class SocketUDP
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
			bool connect(const Endpoint& remoteEndpoint);
			/* Clears the connected endpoint value. */
			void disconnect();

			/* Process arrived packets(onReceive callbacks). */
			void update();

			/* Sends buffer to the connected endpoint. */
			bool sendPacket(const WriteBuffer& buffer);

			/* Sends buffer to a specified endpoint. */
			bool sendPacket(const WriteBuffer& buffer, const Endpoint& endpoint);

			/* Returns false if the memory allocation fails, or the socket is currently receiving data. */
			bool resizeReceiveBuffer(const size_t newSize);

			/* Starts receiving data from the connected endpoint. Non-blocking call. Callback return value specifies whether to keep receiving. */
			bool startReceiving(const std::function<void(ReadBuffer&, const Endpoint& endpoint)> onReceiveCallback);
			void stopReceiving();
			void resumeReceiving();
			void clearReceivedPackets();
			bool isReceiving() const;

			bool isOpen() const;
			Port getLocalPort() const;
			bool isConnected() const;
			Endpoint getConnectedEndpoint() const;			

		private:

			/* Underlying send method for using asio endpoint. */
			bool sendPacketInternal(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);
			
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
				mutable std::recursive_mutex mutex;
				boost::asio::ip::udp::endpoint senderEndpoint;//Used by the receiver thread. Think carefully about thread sync!
				boost::asio::ip::udp::endpoint connectedEndpoint;
				IOService& ioService;
				boost::asio::ip::udp::socket socket;
				std::vector<unsigned char> receiveBuffer;
				time::Time lastReceiveTime;
				bool receiving = false;
				std::function<void(ReadBuffer&, const Endpoint& endpoint)> onReceiveCallback;//User defined receive handler
				std::recursive_mutex receivedPacketsMutex;
				std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;
				SocketUDP* socketUDP = nullptr;
			};
			boost::shared_ptr<SharedImpl> sharedImpl;

		private:
			friend struct SharedImpl;
		};
	}
}