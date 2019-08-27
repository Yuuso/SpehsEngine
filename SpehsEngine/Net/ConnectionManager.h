#pragma once
#include "boost/bind.hpp"
#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Connection.h"
#include <functional>
#include <mutex>


namespace se
{
	namespace net
	{
		/*
			Built on top of UDP.
			Must be able to do the following:
			1. Connection status to the remote endpoint
			2. Option to send packets reliably
				2.1. Reliably received packets are processed in the send order
				2.2. Packets that exceed the MTU must be split in parts

		*/
		class ConnectionManager
		{
		public:

			ConnectionManager(IOService& ioService, const std::string& debugName = "ConnectionManager");
			~ConnectionManager();

			void update();

			/*
				Attempts to start a new outgoing connection.
				May return an empty shared pointer if an error occurs.
				Returns an empty connection if connection was already established.
				(The connection may only be obtained from the entity who originally initiated it.)
			*/
			[[nodiscard]] std::shared_ptr<Connection> startConnecting(const Endpoint& remoteEndpoint, const std::string& _debugName = "Connection");

			/*
				Sets the socket into 'accepting' mode, where incoming connections are accepted.
			*/
			bool startAccepting();
			/*
				Stops accepting any new incoming connections.
				Previously accepted and connecting incoming connections remain connecting.
			*/
			void stopAccepting();
			bool isAccepting() const;
			void connectToIncomingConnectionSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(std::shared_ptr<Connection>&)>& callback);

			//SocketUDP2 wrappers
			bool open();
			void close();
			bool bind(const Port port = Port());
			bool isOpen() const;
			Port getLocalPort() const;
			Endpoint getLocalEndpoint() const;
			size_t getSentBytes() const;
			size_t getReceivedBytes() const;
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

			const std::string debugName;

		private:

			struct ReceivedPacket
			{
				std::vector<uint8_t> data;
				boost::asio::ip::udp::endpoint senderEndpoint;
			};

			void run();

			void receiveHandler(std::vector<uint8_t>& data, const boost::asio::ip::udp::endpoint& senderEndpoint);
			void processReceivedPackets();
			void deliverOutgoingPackets();

			boost::shared_ptr<SocketUDP2> socket;
			mutable std::recursive_mutex mutex;
			std::thread thread;
			bool accepting = false;
			bool destructorCalled = false;
			std::vector<std::shared_ptr<Connection>> connections;
			std::vector<ReceivedPacket> receivedPackets;
			boost::signals2::signal<void(std::shared_ptr<Connection>&)> incomingConnectionSignal;
		};
	}
}
