#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionManager.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Net/AddressUtilityFunctions.h"

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
			}
			thread.join();
			
			//Close socket
			socket->close();
		}

		void ConnectionManager::run()
		{
			setThreadName("ConnectionManager::run()");
			while (true)
			{
				se::time::ScopedFrameLimiter frameLimiter(se::time::fromSeconds(1.0f / 60.0f));
				
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
							if (connections[i]->isConnected() && !connections[i]->reliablePacketSendQueue.empty())
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
					//New incoming connection
					connections.push_back(std::shared_ptr<Connection>(new Connection(socket, receivedPackets[p].senderEndpoint, Connection::EstablishmentType::incoming, "incomingConnection")));
					connections.back()->setDebugLogLevel(getDebugLogLevel());
					connections.back()->receivePacket(receivedPackets[p].data);
					incomingConnectionSignal(connections.back());
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
