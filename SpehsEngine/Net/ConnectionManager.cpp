#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionManager.h"

#include "SpehsEngine/Core/LockGuard.h"
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
					if (connections[i]->isConnected())
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
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t i = 0; i < connections.size(); i++)
			{
				//Remove unreferenced connections that don't have pending operations
				if (connections[i].use_count() == 1 && !connections[i]->hasPendingOperations() && !connections[i]->isConnecting())
				{
					connections.erase(connections.begin() + i);
					i--;
				}
				else
				{
					//Call update
					connections[i]->update();
				}
			}
		}

		void ConnectionManager::processReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t p = 0; p < receivedPackets.size(); p++)
			{
				const std::vector<std::shared_ptr<Connection>>::iterator connectionIt = std::find_if(connections.begin(), connections.end(), [&](std::shared_ptr<Connection>& connection)->bool
					{
						return connection->getRemoteEndpoint() == receivedPackets[p].senderEndpoint;
					});
				if (connectionIt != connections.end())
				{
					//Existing connection
					//se::log::info("Endpoint match: " + se::net::toString(connectionIt->get()->getRemoteEndpoint()) + " == " + se::net::toString(receivedPackets[p].senderEndpoint) + ", size: " + std::to_string(receivedPackets[p].data.size()));
					connectionIt->get()->receivePacket(receivedPackets[p].data);
				}
				else if (accepting)
				{
					// New incoming connection
					connections.push_back(std::shared_ptr<Connection>(new Connection(socket, receivedPackets[p].senderEndpoint, Connection::EstablishmentType::incoming, "incomingConnection")));
					connections.back()->setDebugLogLevel(getDebugLogLevel());

					// The new connection starts in 'connecting' state, wait for it to connect before firing the incomingConnectionSignal
					Connection* const connectionPtr = connections.back().get();
					boost::signals2::scoped_connection& incomingConnectionStatusChangedConnection = incomingConnectionStatusChangedConnections[connectionPtr];
					connections.back()->connectToConnectionStatusChangedSignal(incomingConnectionStatusChangedConnection, [connectionPtr, this](const bool connected)
						{
							const std::unordered_map<Connection*, boost::signals2::scoped_connection>::iterator it = incomingConnectionStatusChangedConnections.find(connectionPtr);
							se_assert(it != incomingConnectionStatusChangedConnections.end());
							incomingConnectionStatusChangedConnections.erase(it);
							if (connected)
							{
								DEBUG_LOG(1, "Incoming connection from: " + connectionPtr->debugEndpoint + " successfully connected.");
								incomingConnectionSignal(connections.back());
							}
							else
							{
								DEBUG_LOG(1, "Incoming connection from: " + connectionPtr->debugEndpoint + " failed to connect.");
							}
						});

					connections.back()->receivePacket(receivedPackets[p].data);
				}
				else
				{
					DEBUG_LOG(1, "Received packet from an untracked endpoint: " + se::net::toString(receivedPackets[p].senderEndpoint));
				}
			}
			receivedPackets.clear();
			for (size_t i = 0; i < connections.size(); i++)
			{
				connections[i]->processReceivedPackets();
			}
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
						return std::shared_ptr<Connection>();
					}
				}

				connections.push_back(std::shared_ptr<Connection>(new Connection(socket, asioEndpoint, Connection::EstablishmentType::outgoing, connectionDebugName)));
				connections.back()->setDebugLogLevel(getDebugLogLevel());
				return connections.back();
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

		size_t ConnectionManager::getSentBytes() const
		{
			return socket->getSentBytes();
		}

		size_t ConnectionManager::getReceivedBytes() const
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
