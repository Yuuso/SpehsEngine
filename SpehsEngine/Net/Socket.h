#pragma once
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/system/error_code.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/signals2.hpp"
#include <type_traits>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/SocketUDP2.h"
#include "SpehsEngine/Core/SE_Time.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		class Socket;
		class IOService;

		struct UnreliablePacket
		{
			std::vector<uint8_t> buffer;
			boost::asio::ip::udp::endpoint endpoint;
		};

		struct ReliablePacket
		{
			std::vector<uint8_t> buffer;
			boost::asio::ip::udp::endpoint endpoint;
			uint8_t controlBits = 0u;
			uint16_t sequenceNumber = 0u;
		};

		class Connection
		{
		public:

			void setReceiveHandler(const std::function<void(ReadBuffer&, const bool, const boost::asio::ip::udp::endpoint&)> callback = std::function<void(ReadBuffer&, const bool, const boost::asio::ip::udp::endpoint&)>());
			bool sendPacket(const WriteBuffer& buffer, const bool reliable);
			void disconnect();
			bool isConnected() const;
			boost::asio::ip::udp::endpoint getRemoteEndpoint() const;

			const boost::asio::ip::udp::endpoint endpoint;
		private:
			friend class Socket;

			Connection(const boost::asio::ip::udp::endpoint& _endpoint) : endpoint(_endpoint) {}
			void receivePacket(std::vector<uint8_t>& data);

			mutable std::recursive_mutex mutex;
			std::vector<UnreliablePacket> receivedUnreliablePackets;
			std::vector<ReliablePacket> receivedReliablePackets;
			bool connected = false;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			size_t sentBytes = 0;
			size_t receivedBytes = 0;
			std::function<void(ReadBuffer&, const bool, const boost::asio::ip::udp::endpoint&)> receiveHandler;
		};
		
		/*
			Built on top of UDP.
			Must be able to do the following:
			1. Connection status to the remote endpoint
			2. Option to send packets reliably
				2.1. Reliably received packets are processed in the send order
				2.2. Packets that exceed the MTU must be split in parts

		*/
		class Socket
		{
		public:

			Socket(IOService& ioService);
			~Socket();

			void update();

			/*
				Starts a new outgoing connection.
			*/
			void startConnecting(const Endpoint& remoteEndpoint);
			void stopConnecting(const boost::asio::ip::udp::endpoint& remoteEndpoint);

			/*
				Sets the socket into 'accepting' mode, where incoming connections are accepted.
			*/
			void startAccepting();
			/*
				Stops accepting and clears all active incoming connection attempts.
			*/
			void stopAccepting();
			bool isAccepting() const;

			void connectToOutgoingConnectionEstablishedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const std::shared_ptr<Connection>&)>& connectionAcceptedCallback);
			void connectToIncomingConnectionEstablishedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const std::shared_ptr<Connection>&)>& connectionAcceptedCallback);

			//SocketUDP2 wrappers
			bool open();
			void close();
			bool bind(const Port& port);
			bool isOpen() const;
			Port getLocalPort() const;
			boost::asio::ip::udp::endpoint getLocalEndpoint() const;
			size_t getSentBytes() const;
			size_t getReceivedBytes() const;
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

		private:

			struct ReceivedPacket
			{
				std::vector<uint8_t> data;
				boost::asio::ip::udp::endpoint senderEndpoint;
			};

			struct IncomingConnection
			{
				std::unique_ptr<Connection> connection;
			};

			struct OutgoingConnection
			{
				std::unique_ptr<Connection> connection;
			};
			
			void receiveHandler(std::vector<uint8_t>& data, const boost::asio::ip::udp::endpoint& senderEndpoint);
			void processReceivedPackets();
			void updateConnections();

			boost::shared_ptr<SocketUDP2> sharedImpl;
			mutable std::recursive_mutex mutex;
			bool accepting = false;
			boost::signals2::signal<void(const std::shared_ptr<Connection>&)> outgoingConnectionEstablishedSignal;
			boost::signals2::signal<void(const std::shared_ptr<Connection>&)> incomingConnectionEstablishedSignal;
			std::vector<std::shared_ptr<Connection>> connections;
			std::vector<std::unique_ptr<IncomingConnection>> incomingConnections;
			std::vector<std::unique_ptr<OutgoingConnection>> outgoingConnections;
			std::vector<ReceivedPacket> receivedPackets;
		};
	}
}
