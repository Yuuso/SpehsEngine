#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
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

			/* Sends buffer to the specified endpoint. */
			bool sendPacket(const WriteBuffer& buffer, const Endpoint& endpoint);

			/* Returns false if the memory allocation fails, or the socket is currently receiving data. */
			bool resizeReceiveBuffer(const size_t newSize);

			/* Starts receiving data from the connected endpoint. Non-blocking call. Callback return value specifies whether to keep receiving. */
			bool startReceiving(const std::function<void(ReadBuffer&, const Endpoint& endpoint)> onReceiveCallback);

			/* Stops receiving data. */
			void stopReceiving();

			/* Returns true if socket is currently able to receive incoming packets. */
			bool isReceiving() const;

			bool isOpen() const;
			Port getLocalPort() const;
			bool isConnected() const;
			Endpoint getConnectedEndpoint() const;
			
			const Id id;

		private:

			/* Underlying send method for using asio endpoint. */
			bool sendPacketInternal(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);

			/* Resumes receiving with the previously set callback handler, not clearing out any arrived packets. */
			void resumeReceiving();

			/* Deallocates and clears received packet buffers. */
			void clearReceivedPackets();

			//Receive handlers
			void receiveHandler(const boost::system::error_code& error, std::size_t bytes);

			mutable std::recursive_mutex mutex;
			IOService& ioService;
			boost::asio::ip::udp::socket* socket = nullptr;
			boost::asio::ip::udp::endpoint senderEndpoint;//Used by the receiver thread. Think carefully about thread sync!
			boost::asio::ip::udp::endpoint connectedEndpoint;
			std::function<void(ReadBuffer&, const Endpoint& endpoint)> onReceiveCallback;//User defined receive handler
			std::vector<unsigned char> receiveBuffer;
			time::Time lastReceiveTime;
			bool receiving = false;

			//Received packets
			struct ReceivedPacket
			{
				std::vector<uint8_t> buffer;
				boost::asio::ip::udp::endpoint senderEndpoint;
			};
			std::recursive_mutex receivedPacketsMutex;
			std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;

		private:
			static Id nextId;
		};
	}
}