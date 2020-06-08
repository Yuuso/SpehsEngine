#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionManager.h"

#include "SpehsEngine/Core/LockGuard.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Net/AddressUtilityFunctions.h"

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
}

/**///#pragma optimize("", off)

namespace se
{
	namespace net
	{
		ConnectionManager::ConnectionManager(IOService& _ioService, const std::string& _debugName)
			: debugName(_debugName)
			, socket(new SocketUDP2(_ioService, _debugName))
			, thread(std::bind(&ConnectionManager::run, this))
		{
			socket->setReceiveHandler(std::bind(&ConnectionManager::receiveHandler, this, std::placeholders::_1, std::placeholders::_2));
		}

		ConnectionManager::~ConnectionManager()
		{
			//Finish async running
			{
				std::lock_guard<std::recursive_mutex> lock1(mutex);
				destructorCalled = true;
				//Queue disconnections for all
				for (size_t i = 0; i < connections.size(); i++)
				{
					if (connections[i]->getStatus() == Connection::Status::Connected)
					{
						connections[i]->disconnect();
					}
				}
			}
			thread.join();

			//Close socket
			socket->close();

			//Log warnings if connections are still referenced somewhere (but will not receive any further network updates)
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t i = 0; i < connections.size(); i++)
			{
				if (connections[i].use_count() > 1)
				{
					log::warning("Connection: " + connections[i]->debugName + " is still referenced somewhere, but will not receive any further updates.");
				}
			}
		}

		void ConnectionManager::run()
		{
			setThreadName("ConnectionManager::run() " + debugName);
			while (true)
			{
				se::time::ScopedFrameLimiter frameLimiter(se::time::fromSeconds(1.0f / 1000.0f));

				//Process incoming data
				socket->update();
				processReceivedPackets();

				//Deliver outgoing data
				deliverOutgoingPackets();

				{
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					if (destructorCalled)
					{
						//Check if there are still pending operations
						bool pendingOperations = false;
						for (size_t i = 0; i < connections.size(); i++)
						{
							if (connections[i]->hasPendingOperations())
							{
								pendingOperations = true;
							}
						}
						if (!pendingOperations)
						{
							break;
						}
					}
				}
			}
		}

		void ConnectionManager::update()
		{
			SE_SCOPE_PROFILER();
			LockGuard lock(mutex);
			const time::Time now = time::now();
			const time::Time timeoutDeltaTime = std::min(now - lastUpdateTime, time::fromSeconds(0.5f));
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t i = 0; i < connections.size(); i++)
			{
				//Remove unreferenced connections that don't have pending operations
				if (connections[i].use_count() == 1 && !connections[i]->hasPendingOperations() && connections[i]->getStatus() == Connection::Status::Connected)
				{
					DEBUG_LOG(1, "Dropping unused alive connection from: " + connections[i]->debugEndpoint + " (" + connections[i]->debugName + ")");
					removeConnectionImpl(i--);
				}
				else
				{
					//Call update
					connections[i]->update(timeoutDeltaTime);
				}
			}
			lastUpdateTime = now;
		}

		void ConnectionManager::processReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t p = 0; p < receivedPackets.size(); p++)
			{
				ReadBuffer readBuffer(receivedPackets[p].data.data(), receivedPackets[p].data.size());
				PacketHeader packetHeader;
				if (packetHeader.read(readBuffer))
				{
					if (packetHeader.protocolId == PacketHeader::spehsProtocolId)
					{
						// Cut header data off from the packet contents
						// TODO: optimize
						const uint16_t rawPacketSize = uint16_t(receivedPackets[p].data.size());
						receivedPackets[p].data.erase(receivedPackets[p].data.begin(), receivedPackets[p].data.begin() + readBuffer.getOffset());

						const std::vector<std::shared_ptr<Connection>>::iterator connectionIt = std::find_if(connections.begin(), connections.end(), [&](std::shared_ptr<Connection>& connection)->bool
							{
								return connection->getRemoteEndpoint() == receivedPackets[p].senderEndpoint;
							});
						if (connectionIt != connections.end())
						{
							//Existing connection
							//se::log::info("Endpoint match: " + se::net::toString(connectionIt->get()->getRemoteEndpoint()) + " == " + se::net::toString(receivedPackets[p].senderEndpoint) + ", size: " + std::to_string(receivedPackets[p].data.size()));
							connectionIt->get()->receivePacket(packetHeader, receivedPackets[p].data, rawPacketSize);
						}
						else if (accepting)
						{
							// New incoming connection
							const std::shared_ptr<Connection> newConnection = addConnectionImpl(std::shared_ptr<Connection>(
								new Connection(socket, receivedPackets[p].senderEndpoint, generateNewConnectionId(), Connection::EstablishmentType::Incoming, debugName + ": Incoming connection")));
							DEBUG_LOG(1, "Incoming connection from: " + newConnection->debugEndpoint + " started connecting...");
							connections.back()->receivePacket(packetHeader, receivedPackets[p].data, rawPacketSize);
						}
						else
						{
							DEBUG_LOG(1, "Received packet from an untracked endpoint: " + se::net::toString(receivedPackets[p].senderEndpoint));
						}
					}
					else
					{
						DEBUG_LOG(1, "Received packet with unrecognized protocol id from: " + se::net::toString(receivedPackets[p].senderEndpoint));
					}
				}
				else
				{
					DEBUG_LOG(1, "Received packet with unrecognized packet header format from: " + se::net::toString(receivedPackets[p].senderEndpoint));
				}
			}
			receivedPackets.clear();
		}

		void ConnectionManager::deliverOutgoingPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t i = 0; i < connections.size(); i++)
			{
				connections[i]->deliverOutgoingPackets();
			}
		}

		std::shared_ptr<Connection> ConnectionManager::startConnecting(const Endpoint& remoteEndpoint, const std::string& connectionDebugName)
		{
			if (!socket->isOpen())
			{
				if (!socket->open())
				{
					log::error("ConnectionManager failed to start connecting, failed to open socket.");
					return std::shared_ptr<Connection>();
				}
			}
			if (!socket->isBound())
			{
				log::error("ConnectionManager failed to start connecting, socket is not bound.");
				return std::shared_ptr<Connection>();
			}
			if (!socket->isReceiving())
			{
				if (!socket->startReceiving())
				{
					log::error("ConnectionManager failed to start connecting, failed to start receiving with the socket.");
					return std::shared_ptr<Connection>();
				}
			}
			const boost::asio::ip::udp::endpoint asioEndpoint = socket->resolveRemoteEndpoint(remoteEndpoint);
			if (asioEndpoint != boost::asio::ip::udp::endpoint())
			{
				std::lock_guard<std::recursive_mutex> lock1(mutex);

				// Return an empty connection if connection was already established
				for (const std::shared_ptr<Connection>& connection : connections)
				{
					if (connection->endpoint == asioEndpoint)
					{
						switch (connection->getStatus())
						{
						case Connection::Status::Connecting:
							log::warning("Connection to " + remoteEndpoint.toString() + " is already being established.");
							break;
						case Connection::Status::Connected:
							log::warning("Connection to " + remoteEndpoint.toString() + " has already been established.");
							break;
						case Connection::Status::Disconnected:
							log::warning("An old connection to " + remoteEndpoint.toString() + " already exists. Wait for it to be removed first...");
							break;
						}
						return std::shared_ptr<Connection>();
					}
				}

				return addConnectionImpl(std::shared_ptr<Connection>(new Connection(
					socket, asioEndpoint, generateNewConnectionId(), Connection::EstablishmentType::Outgoing, connectionDebugName)));
			}
			else
			{
				DEBUG_LOG(1, "failed to start connecting. Endpoint resolution failed.");
				return std::shared_ptr<Connection>();
			}
		}

		bool ConnectionManager::startAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			if (socket->isBound())
			{
				if (isAccepting())
				{
					log::warning("ConnectionManager is already accepting.");
				}
				accepting = true;
				return true;
			}
			else
			{
				log::error("Must bind ConnectionManager first.");
				return false;
			}
		}

		void ConnectionManager::stopAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			se_assert(isAccepting());
			accepting = false;
		}

		bool ConnectionManager::isAccepting() const
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			return accepting;
		}

		void ConnectionManager::connectToIncomingConnectionSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(std::shared_ptr<Connection>&)>& callback)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			scopedConnection = incomingConnectionSignal.connect(callback);
		}

		std::vector<std::shared_ptr<Connection>> ConnectionManager::getConnections()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			return connections;
		}

		void ConnectionManager::receiveHandler(std::vector<uint8_t>& data, const boost::asio::ip::udp::endpoint& senderEndpoint)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			receivedPackets.emplace_back();
			receivedPackets.back().senderEndpoint = senderEndpoint;
			receivedPackets.back().data.swap(data);
		}

		std::shared_ptr<Connection>& ConnectionManager::addConnectionImpl(const std::shared_ptr<Connection>& connection)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			Connection* const connectionPtr = connection.get();
			connections.push_back(connection);
			connections.back()->setDebugLogLevel(getDebugLogLevel());
			connections.back()->setConnectionSimulationSettings(defaultConnectionSimulationSettings);
			connections.back()->connectToStatusChangedSignal(connectionStatusChangedConnections[connectionPtr], [this, connectionPtr](const Connection::Status oldStatus, const Connection::Status newStatus)
				{
					std::lock_guard<std::recursive_mutex> lock1(mutex);
					size_t index = ~0u;
					for (size_t i = 0u; i < connections.size(); i++)
					{
						if (connections[i].get() == connectionPtr)
						{
							index = i;
							break;
						}
					}
					if (index >= connections.size())
					{
						se_assert(false && "Shared connection should exist.");
						return;
					}

					switch (newStatus)
					{
					case Connection::Status::Connecting:
						se_assert(false && "Unexpected status transition.");
						break;

					case Connection::Status::Connected:
						if (connectionPtr->establishmentType == Connection::EstablishmentType::Incoming)
						{
							DEBUG_LOG(1, "Incoming connection from: " + connectionPtr->debugEndpoint + " successfully connected.");
							incomingConnectionSignal(connections[index]);
						}
						break;

					case Connection::Status::Disconnected:
						if (connectionPtr->establishmentType == Connection::EstablishmentType::Incoming)
						{
							if (oldStatus == Connection::Status::Connecting)
							{
								DEBUG_LOG(1, "Incoming connection from: " + connectionPtr->debugEndpoint + " failed to connect.");
							}
						}
						/*
							Stop keeping this connection object alive.
							Users of the shared pointer may hold on to it if they wish, but here we make room for another connection to the same endpoint.
						*/
						removeConnectionImpl(index);
						break;
					}
				});
			return connections.back();
		}

		void ConnectionManager::removeConnectionImpl(const size_t index)
		{
			if (index < connections.size())
			{
				const std::unordered_map<Connection*, boost::signals2::scoped_connection>::iterator connectionStatusChangedConnectionsIt = connectionStatusChangedConnections.find(connections[index].get());
				se_assert(connectionStatusChangedConnectionsIt != connectionStatusChangedConnections.end());
				connectionStatusChangedConnections.erase(connectionStatusChangedConnectionsIt);
				connections.erase(connections.begin() + index);
			}
		}

		ConnectionId ConnectionManager::generateNewConnectionId()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			ConnectionId connectionId;
			while (true)
			{
				connectionId.value = nextConnectionId.value++;
				bool used = false;
				for (std::shared_ptr<Connection>& connection : connections)
				{
					if (connection->connectionId == connectionId)
					{
						used = true;
						break;
					}
				}
				if (connectionId && !used)
				{
					break;
				}
			}
			return connectionId;
		}

		void ConnectionManager::setDefaultConnectionSimulationSettings(const ConnectionSimulationSettings& _defaultConnectionSimulationSettings)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			defaultConnectionSimulationSettings = _defaultConnectionSimulationSettings;
		}

		bool ConnectionManager::open()
		{
			return socket->open();
		}

		void ConnectionManager::close()
		{
			socket->close();
		}

		bool ConnectionManager::bind(const Port port)
		{
			if (socket->open())
			{
				if (socket->bind(port))
				{
					socket->startReceiving();
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		bool ConnectionManager::isOpen() const
		{
			return socket->isOpen();
		}

		Port ConnectionManager::getLocalPort() const
		{
			return socket->getLocalPort();
		}

		Endpoint ConnectionManager::getLocalEndpoint() const
		{
			return Endpoint(getLocalAddress(), getLocalPort());
		}

		uint64_t ConnectionManager::getSentBytes() const
		{
			return socket->getSentBytes();
		}

		uint64_t ConnectionManager::getReceivedBytes() const
		{
			return socket->getReceivedBytes();
		}

		void ConnectionManager::setDebugLogLevel(const int level)
		{
			socket->setDebugLogLevel(level);
		}

		int ConnectionManager::getDebugLogLevel() const
		{
			return socket->getDebugLogLevel();
		}
	}
}
