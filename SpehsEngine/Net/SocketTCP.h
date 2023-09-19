#pragma once

#include "SpehsEngine/Net/ISocket.h"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/PacketMessage.h"
#include "SpehsEngine/Core/SE_Time.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <type_traits>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>


namespace se
{
	class BinaryWriter;
	class BinaryReader;

	namespace net
	{
		namespace aria
		{
			class Connector;
			class Client;
		}
		class SocketTCP : public ISocket
		{
		private:
			typedef uint32_t ExpectedBytesType;
			typedef uint32_t Id;
			friend class aria::Connector;
			friend class aria::Client;

		public:
			enum class AcceptingState { idle, listeningForConnection, establishingConnection };

		public:

			SocketTCP(IOService& ioService);
			virtual ~SocketTCP();

			/* Opens the socket with an available port. */
			bool open();
			void close();
			bool bind(const Port& port);

			/*
				Process incoming connections(onAccept callbacks).
				Process arrived packets(onReceive callbacks).
			*/
			void update();

			/* Perform a synchronous connection attempt. */
			bool connect(const Endpoint& endpoint);

			/* Disconnects the current connection. Socket still remains open, bound to its port. */
			void disconnect();

			/* Sends buffer to the connected endpoint. Spehs-level packet type specification is also possible (only for advanced use). */
			bool sendPacket(const BinaryWriter& BinaryWriter, const PacketType packetType = PacketType::undefined);

			/* Try placing a packet into the receive queue to be processed as if it was received through the connection. Returns false if not connected. */
			bool placeDataOnReceiveQueue(BinaryWriter& binaryWriter);

			/* Returns false if the memory allocation fails, or the socket is currently receiving data. */
			bool resizeReceiveBuffer(const size_t newSize);

			/* Socket keeps receiving packets as long as connection is alive. Received packets must be processed with a specified receive handler. */
			void setOnReceiveCallback(const std::function<void(BinaryReader&)> onReceiveCallback = std::function<void(BinaryReader&)>());
			bool hasOnReceiveCallback() const;

			/* Starts listening for a new incoming connection. Upon success, a connection is made. Non-blocking call. Callback is called even if no connection is made! */
			bool startAccepting(const Port& port, const std::function<void(SocketTCP&)> onAcceptCallback);
			
			/* Stops accepting an incoming connection. */
			void stopAccepting();

			/* Enabling the 'no delay' option disables Nagle's algorithm. */
			void setNoDelay(const bool enabled);
			bool getNoDelay() const;

			void setReuseAddress(const bool enabled);
			bool getReuseAddress() const;

			/* Enabling the 'keep alive' option causes socket to send keep-alive messages. */
			void setKeepAlive(const bool enabled);
			bool getKeepAlive() const;

			/* Returns the total number of sent bytes. Does not account bytes from the IP and TCP implementation. */
			size_t getSentBytes() const override;

			/* Returns the total number of received bytes. Does not account bytes from the IP and TCP implementation. */
			size_t getReceivedBytes() const override;

			/* Remote endpoint. */
			Address getRemoteAddress() const;
			Port getRemotePort() const;
			boost::asio::ip::tcp::endpoint getRemoteEndpoint() const;

			bool isOpen() const;
			Port getLocalPort() const;
			boost::asio::ip::tcp::endpoint getLocalEndpoint() const;

			AcceptingState getAcceptingState() const;
			/* Returns true if the socket is currently listening for an incoming connection on a port, or the accepted connection is currently being established. */
			bool isAccepting() const;
			/* Returns true if socket is currently able to receive incoming packets. */
			bool isReceiving() const;
			/* Returns true if the socket is currently connected to a remote spehs socket. */
			bool isConnected() const;

			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

		private:

			void startReceiving();

			/* Disconnect the socket with the specified type */
			void disconnect(const DisconnectType disconnectType);

			/* Blocks until receiving has stopped. */
			void waitUntilFinishedReceiving();

			/* Blocks until accepting has stopped. */
			void waitUntilFinishedAccepting();

			/* Blocks until handshakeReceived==true, or time is out. */
			bool waitForHandshake(const time::Time timeout);

			/* Deallocates and clears received packet buffers. */
			void clearReceivedPackets();
			
			//Receive handlers
			bool spehsReceiveHandler(BinaryReader& buffer);//Internal receive handler, unpacks spehs header. Calls the user defined receive handler.

			struct SharedImpl : public boost::enable_shared_from_this<SharedImpl>
			{
				SharedImpl(IOService& ioService);
				~SharedImpl();

				bool open();
				void close();
				bool bind(const Port& port);
				/* Boost initially passes received data to this receive handler. */
				void receiveHandler(const boost::system::error_code& error, std::size_t bytes);
				bool startAccepting(const Port& port, const std::function<void(SocketTCP&)> callbackFunction);
				/* Boost acceptor calls this method when an incoming connection is being accepted. If no error is detected, launches the synchronous spehsAccept method in a different thread (spehsAcceptThread). */
				void onAccept(const boost::system::error_code error);		
				void spehsAccept();
				bool waitForHandshake(const time::Time timeout);
				void startReceiving();
				void clearReceivedPackets();
				void stopAccepting();
				bool connect(const Endpoint& endpoint);
				void disconnect();
				void disconnect(const DisconnectType disconnectType);
				bool sendPacket(const BinaryWriter& binaryWriter, const PacketType packetType);
				bool placeDataOnReceiveQueue(BinaryWriter& binaryWriter);
				bool resizeReceiveBuffer(const size_t newSize);
				void setOnReceiveCallback(const std::function<void(BinaryReader&)> callbackFunction);
				bool hasOnReceiveCallback() const;
				void update();
				bool spehsReceiveHandler(BinaryReader& binaryReader);
				void setNoDelay(const bool enabled);
				bool getNoDelay() const;
				void setReuseAddress(const bool enabled);
				bool getReuseAddress() const;
				void setKeepAlive(const bool enabled);
				bool getKeepAlive() const;

				Address getRemoteAddress() const;
				Port getRemotePort() const;
				boost::asio::ip::tcp::endpoint getRemoteEndpoint() const;
				AcceptingState getAcceptingState() const;
				bool isAccepting() const;
				bool isReceiving() const;
				bool isConnected() const;
				bool isOpen() const;
				Port getLocalPort() const;
				boost::asio::ip::tcp::endpoint getLocalEndpoint() const;

				size_t getSentBytes() const;
				size_t getReceivedBytes() const;

				mutable std::recursive_mutex mutex;
				IOService& ioService;
				boost::asio::ip::tcp::socket socket;
				std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
				std::unique_ptr<std::thread> spehsAcceptThread;
				std::function<void(BinaryReader&)> onReceiveCallback; // User defined receive handler
				std::function<void(SocketTCP&)> onAcceptCallback;
				ExpectedBytesType expectedBytes = 0;
				std::vector<unsigned char> receiveBuffer;
				time::Time lastReceiveTime;
				bool receiving = false;
				AcceptingState acceptingState = AcceptingState::idle;
				bool connected = false;
				bool connecting = false;//Set to true for the duration of connect attempt
				bool handshakeSent = false;//Refers to the current connection
				bool handshakeReceived = false;//Refers to the current connection
				bool onAcceptCallbackQueued = false;//If enabled, update will invoke the callback.
				bool destructorCalled = false;
				SocketTCP* socketTCP = nullptr;
				size_t sentBytes = 0;
				size_t receivedBytes = 0;

				//Received packets
				std::recursive_mutex receivedPacketsMutex;
				struct ReceivedPacket
				{
					ReceivedPacket() = default;
					ReceivedPacket(const size_t size) : data(size) {}
					std::vector<uint8_t> data;
				};
				std::vector<std::unique_ptr<ReceivedPacket>> receivedPackets;

				/*
					level 1: prints most essential state changes.
					level 2: prints some network traffic numbers.
					level 3: prints receive buffer in hex string.
				*/
				int debugLogLevel = 0;
				void setDebugLogLevel(const int level);
				int getDebugLogLevel() const;
			};
			boost::shared_ptr<SharedImpl> sharedImpl;

		private:
			friend struct SharedImpl;
		};

		std::string toString(const boost::asio::ip::tcp::endpoint& endpoint);
	}
}
