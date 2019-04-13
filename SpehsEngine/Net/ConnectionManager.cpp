#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionManager.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
}

/**///#pragma optimize("", off)

namespace
{
	typedef uint16_t SizeType;
#ifdef SE_FINAL_RELEASE
	static const se::time::Time connectionTimeout = se::time::fromSeconds(5.0f);
#else
	static const se::time::Time connectionTimeout = se::time::fromSeconds(10000.0f);
#endif
}

namespace se
{
	namespace net
	{
		ConnectionManager::ConnectionManager(IOService& _ioService, const ProtocolId _protocolId, const std::string& _debugName)
			: protocolId(_protocolId)
			, debugName(_debugName)
			, socket(new SocketUDP2(_ioService, _debugName))
		{
			se_assert(protocolId);
			socket->setReceiveHandler(std::bind(&ConnectionManager::receiveHandler, this, std::placeholders::_1, std::placeholders::_2));
		}

		ConnectionManager::~ConnectionManager()
		{
			socket->close();
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			//One last update to lose the connection status
			for (size_t i = 0; i < connections.size(); i++)
			{
				connections[i]->update();
			}
		}

		void ConnectionManager::update()
		{
			socket->update();
			processReceivedPackets();
			updateConnections();
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
					connectionIt->get()->receivePacket(receivedPackets[p].data);
				}
				else if (accepting)
				{
					//New incoming connection
					connections.push_back(std::shared_ptr<Connection>(new Connection(socket, receivedPackets[p].senderEndpoint, protocolId, Connection::EstablishmentType::incoming, "incomingConnection")));
					connections.back()->setDebugLogLevel(getDebugLogLevel());
					connections.back()->receivePacket(receivedPackets[p].data);
					incomingConnectionSignal(connections.back());
				}
			}
			receivedPackets.clear();
		}

		void ConnectionManager::updateConnections()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);

			for (size_t i = 0; i < connections.size(); i++)
			{
				//Remove disconnected and unreferenced connections
				if (connections[i].use_count() == 1 && !connections[i]->isConnected() && !connections[i]->isConnecting())
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

		std::shared_ptr<Connection> ConnectionManager::startConnecting(const Endpoint& remoteEndpoint, const std::string& connectionDebugName)
		{
			if (!isOpen())
			{
				if (!open())
				{
					return std::shared_ptr<Connection>();
				}
			}
			if (!socket->isReceiving())
			{
				socket->startReceiving();
			}
			const boost::asio::ip::udp::endpoint asioEndpoint = socket->resolveRemoteEndpoint(remoteEndpoint);
			if (asioEndpoint != boost::asio::ip::udp::endpoint())
			{
				std::lock_guard<std::recursive_mutex> lock1(mutex);
				connections.push_back(std::shared_ptr<Connection>(new Connection(socket, asioEndpoint, protocolId, Connection::EstablishmentType::outgoing, connectionDebugName)));
				connections.back()->setDebugLogLevel(getDebugLogLevel());
				return connections.back();
			}
			else
			{
				DEBUG_LOG(1, "failed to start connecting. Endpoint resolution failed.");
				return std::shared_ptr<Connection>();
			}
		}

		void ConnectionManager::startAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			se_assert(!isAccepting());
			accepting = true;
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

		bool ConnectionManager::bind(const Port& port)
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

		boost::asio::ip::udp::endpoint ConnectionManager::getLocalEndpoint() const
		{
			return socket->getLocalEndpoint();
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
