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
#include "SpehsEngine/Net/PacketMessage.h"
#include "SpehsEngine/Core/SE_Time.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		class HandshakeUDP;

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
			bool connect(const Endpoint& remoteEndpoint);
			/* Clears the connected endpoint value. */
			void disconnect();

			/* Process arrived packets(onReceive callbacks). */
			void update();

			/* Sends buffer to the set remote endpoint. */
			bool sendPacket(const WriteBuffer& buffer, const PacketType packetType = PacketType::undefined);

			/* Sends buffer to a specified endpoint. */
			bool sendPacket(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint, const PacketType packetType = PacketType::undefined);

			/* Returns false if the memory allocation fails, or the socket is currently receiving data. */
			bool resizeReceiveBuffer(const size_t newSize);

			/* Starts receiving data from the connected endpoint. Non-blocking call. */
			bool startReceiving();
			bool isReceiving() const;

			/* Heartbeat packets are sent between set intervals (requires calling update()). When set to 0, no heartbeats are sent. */
			void setHeartbeatInterval(const time::Time interval);
			time::Time getHeartbeatInterval() const;
			
			/* Returns the total number of sent bytes. Does not account bytes from the IP implementation. */
			size_t getSentBytes() const override;

			/* Returns the total number of received bytes. Does not account bytes from the IP implementation. */
			size_t getReceivedBytes() const override;

			/* Received packets must be processed with a specified receive handler. While there exists no callback, all incoming packets are discarded. */
			void setOnReceiveCallback(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&)> onReceiveCallback = std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&)>());

			void setReuseAddress(const bool enabled);
			bool getReuseAddress() const;

			bool isOpen() const;
			Port getLocalPort() const;

			bool isConnected() const;
			boost::asio::ip::udp::endpoint getLocalEndpoint() const;
			boost::asio::ip::udp::endpoint getConnectedEndpoint() const;

			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

		private:

			void resumeReceiving();
			void clearReceivedPackets();
			
			//Received packets
			struct SharedImpl : public boost::enable_shared_from_this<SharedImpl>
			{
				SharedImpl(IOService& ioService);
				bool sendPacket(const WriteBuffer& buffer, const boost::asio::ip::udp::endpoint& endpoint, const PacketType packetType);
				void receiveHandler(const boost::system::error_code& error, std::size_t bytes);
				boost::asio::ip::udp::endpoint getLocalEndpoint() const;
				Port getLocalPort() const;
				void setDebugLogLevel(const int level);
				int getDebugLogLevel() const;

				struct ReceivedPacket
				{
					std::vector<uint8_t> buffer;
					boost::asio::ip::udp::endpoint senderEndpoint;
				};
				mutable std::recursive_mutex mutex;
				boost::asio::ip::udp::endpoint connectedEndpoint;
				boost::asio::ip::udp::endpoint senderEndpoint;//Used by the receiver thread. Think carefully about thread sync!
				IOService& ioService;
				boost::asio::ip::udp::socket socket;
				std::vector<unsigned char> receiveBuffer;
				time::Time lastReceiveTime;
				time::Time lastSendTime;
				time::Time heartbeatInterval = se::time::fromSeconds(5.0f);
				bool receiving = false;
				std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&)> onReceiveCallback;//User defined receive handler
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
			boost::shared_ptr<SharedImpl> sharedImpl;

		private:
			friend struct SharedImpl;
		};
	}
}

