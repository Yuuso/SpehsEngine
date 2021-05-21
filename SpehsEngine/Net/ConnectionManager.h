#pragma once
#include "boost/bind.hpp"
#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Connection.h"
#include "SpehsEngine/Net/SocketUDP2.h"
#include <functional>
#include <unordered_map>
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
				Runs update() on loop until no connection has reliable data in send queue or specified timeout time has elapsed.
				Returns true on success. Returns false on timeout.
			*/
			bool updateUntilNoReliablePacketsInSendQueue(const se::time::Time timeout = se::time::Time::zero);

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
			std::vector<std::shared_ptr<Connection>> getConnections();

			//SocketUDP2 wrappers
			bool open();
			void close();
			bool bind(const Port port = Port());
			bool isOpen() const;
			bool hasPendingOperations() const;
			Port getLocalPort() const;
			Endpoint getLocalEndpoint() const;
			uint64_t getSentBytes() const;
			uint64_t getReceivedBytes() const;
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;
			void setDefaultConnectionSimulationSettings(const ConnectionSimulationSettings& _defaultConnectionSimulationSettings);

			const std::string debugName;

		private:
			
			void run();
			void receiveHandler(ReceivedPacketSocketUDP2& receivedPacket);
			void processReceivedPackets();
			void processReceivedPacket(std::lock_guard<std::recursive_mutex>& lock, const PacketHeader& packetHeader, const boost::asio::ip::udp::endpoint &senderEndpoint, std::vector<uint8_t>& buffer, const size_t bufferOffset);
			void deliverOutgoingPackets();
			ConnectionId generateNewConnectionId();
			std::shared_ptr<Connection>& addConnectionImpl(const std::shared_ptr<Connection> &connection);
			void removeConnectionImpl(const std::vector<std::shared_ptr<Connection>>::iterator it);

			boost::shared_ptr<SocketUDP2> socket;
			mutable std::shared_ptr<std::recursive_mutex> mutex;
			std::thread thread;
			bool accepting = false;
			bool destructorCalled = false;
			time::Time lastUpdateTime;
			std::vector<std::shared_ptr<Connection>> connections;
			std::vector<ReceivedPacketSocketUDP2> receivedPackets;
			boost::signals2::signal<void(std::shared_ptr<Connection>&)> incomingConnectionSignal;
			std::unordered_map<ConnectionId, boost::signals2::scoped_connection> connectionStatusChangedConnections;

			ConnectionSimulationSettings defaultConnectionSimulationSettings;
		};
	}
}
