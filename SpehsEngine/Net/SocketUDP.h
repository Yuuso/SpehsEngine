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

			/* Opens and binds the socket. Must be called before packets can be received or sent. */
			bool open(const Port& port);
			bool isOpen() const;
			Port getLocalPort() const;

			void close();

			/* Process arrived packets(onReceive callbacks). */
			void update();

			/* Sends buffer to the default send to endpoint. */
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

			/* UDP is a connection protocol. However, a default send to endpoint can be set. */
			void setDefaultSendToEndpoint(const Endpoint& defaultSendToEndpoint);
			bool isDefaultSendToEndpointSet() const;
			
			/* Prevents other threads from using this socket. Incoming receive callbacks will have to wait until the thread lock is released. */
			void enableThreadLock() { mutex.lock(); }
			/* Releases previously enabled thread lock. */
			void releaseThreadLock() { mutex.unlock(); }

			const Id id;

		private:

			/* Resumes receiving with the previously set callback handler, not clearing out any arrived packets. */
			void resumeReceiving();

			/* Blocks until receiving has stopped. */
			void waitUntilFinishedReceiving();

			/* Deallocates and clears received packet buffers. */
			void clearReceivedPackets();

			//Receive handlers
			void receiveHandler(const boost::system::error_code& error, std::size_t bytes);

			mutable std::recursive_mutex mutex;
			IOService& ioService;
			boost::asio::ip::udp::socket socket;
			boost::asio::ip::udp::endpoint senderEndpoint;//Used the receiver thread. Think carefully about thread sync!
			Endpoint defaultSendToEndpoint;
			std::function<void(ReadBuffer&, const Endpoint& endpoint)> onReceiveCallback;//User defined receive handler
			std::vector<unsigned char> receiveBuffer;
			time::Time lastReceiveTime;
			bool receiving = false;

			//Received packets
			struct ReceivedPacket
			{
				std::vector<uint8_t> buffer;
				Endpoint senderEndpoint;
			};
			std::recursive_mutex receivedPacketsMutex;
			std::vector<ReceivedPacket*> receivedPackets;

		private:
			static Id nextId;
		};
	}
}