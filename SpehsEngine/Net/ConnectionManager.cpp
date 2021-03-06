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
#include <set>

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
}


namespace se
{
	namespace net
	{
		ConnectionManager::ConnectionManager(IOService& _ioService, const std::string& _debugName)
			: debugName(_debugName)
			, socket(new SocketUDP2(_ioService, _debugName))
			, mutex(new std::recursive_mutex())
			, thread(std::bind(&ConnectionManager::run, this))
		{
			socket->setReceiveHandler(std::bind(&ConnectionManager::receiveHandler, this, std::placeholders::_1));
		}

		ConnectionManager::~ConnectionManager()
		{
			//Finish async running
			{
				std::lock_guard<std::recursive_mutex> lock1(*mutex);
				destructorCalled = true;
			}
			thread.join();

			//Close socket
			socket->close();

			//Log warnings if connections are still referenced somewhere (but will not receive any further network updates)
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
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
			bool keepRunning = true;
			while (keepRunning)
			{
				se::time::ScopedFrameLimiter frameLimiter(se::time::fromSeconds(1.0f / 1000.0f));

				//Process incoming data
				socket->update();
				processReceivedPackets();

				//Deliver outgoing data
				deliverOutgoingPackets();

				{
					std::lock_guard<std::recursive_mutex> lock1(*mutex);
					if (destructorCalled)
					{
						keepRunning = false;
					}
				}
			}
		}

		void ConnectionManager::update()
		{
			SE_SCOPE_PROFILER();
			LockGuard lock(*mutex);
			const time::Time now = time::now();
			const time::Time timeoutDeltaTime = std::min(now - lastUpdateTime, time::fromSeconds(0.5f));
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			for (size_t i = 0; i < connections.size(); i++)
			{
				const Connection::Status connectionStatus = connections[i]->getStatus();
				// Remove unreferenced connections that don't have pending operations
				if (connectionStatus == Connection::Status::Connected && connections[i].use_count() == 1 && !connections[i]->hasPendingOperations())
				{
					DEBUG_LOG(1, "Disconnecting unused alive connection from: " + connections[i]->debugEndpoint + " (" + connections[i]->debugName + ")");
					connections[i]->queueDisconnect();
				}
				else if (connectionStatus == Connection::Status::Disconnected)
				{
					/*
						Make room for a new connection from the same endpoint.
						NOTE: do not handle this in the connection status changed callback,
							as that callback may be called during internal operations,
							in the middle of an update loop where the connections vector is being iterated.
					*/
					removeConnectionImpl(connections.begin() + i);
					i--;
					continue;
				}

				connections[i]->update(timeoutDeltaTime);
			}
			lastUpdateTime = now;
		}

		bool ConnectionManager::updateUntilNoReliablePacketsInSendQueue(const se::time::Time timeout)
		{
			std::optional<time::Time> timeoutTime;
			if (timeout != se::time::Time::zero)
			{
				timeoutTime.emplace(time::now() + timeout);
			}

			const time::Time frameTime = time::fromSeconds(1.0f / 60.0f);
			while (true)
			{
				update();

				const time::ScopedFrameLimiter frameLimiter(frameTime);
				bool ready = true;
				{
					std::lock_guard<std::recursive_mutex> lock1(*mutex);
					for (std::shared_ptr<Connection>& connection : connections)
					{
						if (connection->hasReliableUnacknowledgedBytesInQueue(1ull))
						{
							ready = false;
							break;
						}
					}
				}

				if (ready)
				{
					return true;
				}

				if (timeoutTime && time::now() < *timeoutTime)
				{
					return false;
				}
			}			
		}

		void ConnectionManager::processReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			for (ReceivedPacketSocketUDP2& receivedPacket : receivedPackets)
			{
				if (receivedPacket.errorType == ReceivedPacketSocketUDP2::ErrorType::None)
				{
					ReadBuffer readBuffer(receivedPacket.buffer.data(), receivedPacket.buffer.size());
					PacketHeader packetHeader;
					if (packetHeader.read(readBuffer)) // TODO: buffer may be empty, check the connectionRefused bool
					{
						if (packetHeader.protocolId == PacketHeader::spehsProtocolId)
						{
							processReceivedPacket(lock1, packetHeader, receivedPacket.senderEndpoint, receivedPacket.buffer, readBuffer.getOffset());
						}
						else
						{
							DEBUG_LOG(1, "Received packet with unrecognized protocol id from: " + se::net::toString(receivedPacket.senderEndpoint));
						}
					}
					else
					{
						DEBUG_LOG(1, "Received packet with unrecognized packet header format from: " + se::net::toString(receivedPacket.senderEndpoint));
					}
				}
				else
				{
					switch (receivedPacket.errorType)
					{
					case ReceivedPacketSocketUDP2::ErrorType::None:
						break;
					case ReceivedPacketSocketUDP2::ErrorType::ConnectionRefused:
						// Disconnect
						for (const std::shared_ptr<Connection>& connection : connections)
						{
							if (connection->getRemoteEndpoint() == receivedPacket.senderEndpoint)
							{
								se::LockGuard lock2(connection->mutex);
								Connection::Status connectionStatus = connection->getStatus();
								if (connectionStatus == Connection::Status::Connected || connectionStatus == Connection::Status::Disconnecting)
								{
									connection->setStatus(Connection::Status::Disconnected, lock1, lock2);
								}
							}
						}
						break;
					}
				}
			}
			receivedPackets.clear();
		}

		void ConnectionManager::processReceivedPacket(std::lock_guard<std::recursive_mutex>& lock1,
			const PacketHeader& packetHeader, const boost::asio::ip::udp::endpoint& senderEndpoint, std::vector<uint8_t>& buffer, const size_t bufferOffset)
		{
			se_assert(packetHeader.connectionId);
			Connection* connectionWithMatchingConnectionId = nullptr;
			Connection* connectionWithMatchingEndpointAndConnectionIdless = nullptr;
			for (const std::shared_ptr<Connection>& connection : connections)
			{
				if (connection->getRemoteEndpoint() == senderEndpoint)
				{
					if (connection->remoteConnectionId)
					{
						if (connection->remoteConnectionId == packetHeader.connectionId)
						{
							connectionWithMatchingConnectionId = connection.get();
						}
					}
					else
					{
						connectionWithMatchingEndpointAndConnectionIdless = connection.get();
					}
				}
			}

			Connection* connection = nullptr;
			if (connectionWithMatchingConnectionId)
			{
				connection = connectionWithMatchingConnectionId;
			}
			else if (connectionWithMatchingEndpointAndConnectionIdless)
			{
				connection = connectionWithMatchingEndpointAndConnectionIdless;
			}

			bool isNewConnection = true;
			if (connection)
			{
				// Existing connection
				if (!connection->remoteConnectionId || connection->remoteConnectionId == packetHeader.connectionId)
				{
					connection->receivePacket(packetHeader, buffer, bufferOffset);
					isNewConnection = false;
				}
				else if (se::time::now() - connection->lastReceiveTime < se::time::fromSeconds(5.0f))
				{
					// Possibly a packet from an older/newer connection, ignore for now.
					isNewConnection = false;
				}
				else
				{
					// The old connection died...?
					DEBUG_LOG(1, "Received packet with mismatching connection id from: " + se::net::toString(senderEndpoint) + ". Disconnecting the old connection.");
					se::LockGuard lock2(connection->mutex);
					connection->setStatus(Connection::Status::Disconnected, lock1, lock2);
				}
			}

			if (isNewConnection)
			{
				if (accepting)
				{
					// New incoming connection
					const std::shared_ptr<Connection> newConnection = addConnectionImpl(std::shared_ptr<Connection>(
						new Connection(socket, mutex, senderEndpoint, generateNewConnectionId(), packetHeader.connectionId, debugName + ": Incoming connection")));
					DEBUG_LOG(1, "Incoming connection from: " + newConnection->debugEndpoint + " started connecting with connection id: " + std::to_string(packetHeader.connectionId.value));
					newConnection->receivePacket(packetHeader, buffer, bufferOffset);
				}
				else
				{
					DEBUG_LOG(1, "Received packet from untracked endpoint: " + se::net::toString(senderEndpoint) + ". ConnectionManager is not in accepting state, ignoring.");
				}
			}
		}

		void ConnectionManager::deliverOutgoingPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
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
				std::lock_guard<std::recursive_mutex> lock1(*mutex);

				// Return an empty connection if connection is currently being established
				for (const std::shared_ptr<Connection>& connection : connections)
				{
					if (connection->endpoint == asioEndpoint && connection->getStatus() == Connection::Status::Connecting)
					{
						return std::shared_ptr<Connection>();
					}
				}

				return addConnectionImpl(std::shared_ptr<Connection>(new Connection(
					socket, mutex, asioEndpoint, generateNewConnectionId(), ConnectionId(), connectionDebugName)));
			}
			else
			{
				DEBUG_LOG(1, "failed to start connecting. Endpoint resolution failed.");
				return std::shared_ptr<Connection>();
			}
		}

		bool ConnectionManager::startAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
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
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			se_assert(isAccepting());
			accepting = false;
		}

		bool ConnectionManager::isAccepting() const
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			return accepting;
		}

		void ConnectionManager::connectToIncomingConnectionSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(std::shared_ptr<Connection>&)>& callback)
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			scopedConnection = incomingConnectionSignal.connect(callback);
		}

		std::vector<std::shared_ptr<Connection>> ConnectionManager::getConnections()
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			return connections;
		}

		void ConnectionManager::receiveHandler(ReceivedPacketSocketUDP2& receivedPacket)
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			receivedPackets.emplace_back();
			std::swap(receivedPackets.back(), receivedPacket);
		}

		std::shared_ptr<Connection>& ConnectionManager::addConnectionImpl(const std::shared_ptr<Connection>& connection)
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			const ConnectionId connectionId = connection->connectionId;
			connections.push_back(connection);
			connections.back()->setDebugLogLevel(getDebugLogLevel());
			connections.back()->setConnectionSimulationSettings(defaultConnectionSimulationSettings);
			connections.back()->connectToStatusChangedSignal(connectionStatusChangedConnections[connectionId], [this, connectionId](const Connection::Status oldStatus, const Connection::Status newStatus)
				{
					std::lock_guard<std::recursive_mutex> lock1(*mutex);
					size_t index = ~0u;
					for (size_t i = 0u; i < connections.size(); i++)
					{
						if (connections[i]->connectionId == connectionId)
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
						if (connections[index]->establishmentType == Connection::EstablishmentType::Incoming)
						{
							DEBUG_LOG(1, "Incoming connection from: " + connections[index]->debugEndpoint + " successfully connected.");
							incomingConnectionSignal(connections[index]);
						}
						break;

					case Connection::Status::Disconnecting:
						break;

					case Connection::Status::Disconnected:
						if (connections[index]->establishmentType == Connection::EstablishmentType::Incoming)
						{
							if (oldStatus == Connection::Status::Connecting)
							{
								DEBUG_LOG(1, "Incoming connection from: " + connections[index]->debugEndpoint + " failed to connect.");
							}
						}
						break;
					}
				});
			return connections.back();
		}

		void ConnectionManager::removeConnectionImpl(const std::vector<std::shared_ptr<Connection>>::iterator it)
		{
			const std::unordered_map<ConnectionId, boost::signals2::scoped_connection>::iterator connectionStatusChangedConnectionsIt = connectionStatusChangedConnections.find(it->get()->connectionId);
			se_assert(connectionStatusChangedConnectionsIt != connectionStatusChangedConnections.end());
			connectionStatusChangedConnections.erase(connectionStatusChangedConnectionsIt);
			connections.erase(it);
		}

		ConnectionId ConnectionManager::generateNewConnectionId()
		{
			// Keep a list of randomly generated connection ids during this program run to minimize clashes
			static std::set<ConnectionId::ValueType> usedConnectionIds;
			static std::mutex usedConnectionIdsMutex;

			ConnectionId connectionId;
			while (!connectionId || usedConnectionIds.find(connectionId.value) != usedConnectionIds.end())
			{
				connectionId.value = rng::random<ConnectionId::ValueType>();
			}

			usedConnectionIds.insert(connectionId.value);
			return connectionId;
		}

		void ConnectionManager::setDefaultConnectionSimulationSettings(const ConnectionSimulationSettings& _defaultConnectionSimulationSettings)
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
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

		bool ConnectionManager::hasPendingOperations() const
		{
			std::lock_guard<std::recursive_mutex> lock1(*mutex);
			for (size_t i = 0; i < connections.size(); i++)
			{
				if (connections[i]->hasPendingOperations())
				{
					return true;
				}
			}
			return false;
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
