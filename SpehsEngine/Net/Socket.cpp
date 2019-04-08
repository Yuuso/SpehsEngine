#include "stdafx.h"
#include "SpehsEngine/Net/Socket.h"

#include <assert.h>
#include <iostream>
#include <string>
#include "SpehsEngine/Net/IOService.h"
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>
#include "SpehsEngine/Core/RAIIVariableSetter.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StringOperations.h"

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info("Socket(" + getLocalPort().toString() + "): " + message); \
}

/**/#pragma optimize("", off)

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
		void Connection::setReceiveHandler(const std::function<void(ReadBuffer&, const bool, const boost::asio::ip::udp::endpoint&)> callback)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			receiveHandler = callback;		
		}

		bool Connection::sendPacket(const WriteBuffer& buffer, const bool reliable)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return false;
		}

		void Connection::disconnect()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
		}

		void Connection::receivePacket(std::vector<uint8_t>& data)
		{

		}

		bool Connection::isConnected() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return connected;
		}
		
		boost::asio::ip::udp::endpoint Connection::getRemoteEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return endpoint;
		}




		// Socket
		Socket::Socket(IOService& ioService)
			: sharedImpl(new SocketUDP2(ioService))
		{

		}

		Socket::~Socket()
		{
			sharedImpl->close();
		}

		void Socket::update()
		{
			sharedImpl->update();
			processReceivedPackets();
			updateConnections();
		}

		void Socket::processReceivedPackets()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t p = 0; p < receivedPackets.size(); p++)
			{
				const std::vector<std::shared_ptr<Connection>>::iterator it = std::find_if(connections.begin(), connections.end(), [&](std::shared_ptr<Connection>& connection)->bool
				{ return connection->getRemoteEndpoint() == receivedPackets[p].senderEndpoint; });
				if (it != connections.end())
				{
					//Existing connection
					it->get()->receivePacket(receivedPackets[p].data);
				}
				else
				{
					//New connection
					if (accepting)
					{
						incomingConnections.push_back(std::make_unique<IncomingConnection>());
						incomingConnections.back()->connection.reset(new Connection(receivedPackets[p].senderEndpoint));
						incomingConnections.back()->connection->receivePacket(receivedPackets[p].data);
					}
				}

				for (size_t c = 0; c < connections.size(); c++)
				{
					if (connections[c]->getRemoteEndpoint() == receivedPackets[p].senderEndpoint)
					{

					}
				}
			}
		}

		void Socket::updateConnections()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);

			//Remove disconnected and unreferenced connections
			for (size_t i = 0; i < connections.size(); i++)
			{
				if (connections[i].use_count() == 1 && !connections[i]->isConnected())
				{
					connections.erase(connections.begin() + i);
					i--;
				}
			}
		}

		void Socket::startConnecting(const Endpoint& remoteEndpoint)
		{
			const boost::asio::ip::udp::endpoint asioEndpoint = sharedImpl->resolveRemoteEndpoint(remoteEndpoint);
			if (asioEndpoint != boost::asio::ip::udp::endpoint())
			{
				std::lock_guard<std::recursive_mutex> lock1(mutex);
				se_assert(isOpen());
				outgoingConnections.push_back(std::make_unique<OutgoingConnection>());
				outgoingConnections.back()->connection.reset(new Connection(asioEndpoint));
			}
			else
			{
				DEBUG_LOG(1, "failed to start connecting. Endpoint resolution failed.");
			}
		}

		void Socket::stopConnecting(const boost::asio::ip::udp::endpoint& remoteEndpoint)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			for (size_t i = 0; i < outgoingConnections.size(); i++)
			{
				if (outgoingConnections[i]->connection->endpoint == remoteEndpoint)
				{
					outgoingConnections.erase(outgoingConnections.begin() + i);
					return;
				}
			}
		}

		void Socket::startAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			se_assert(!isAccepting());
			accepting = true;
		}

		void Socket::stopAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			se_assert(isAccepting());
			accepting = false;
			incomingConnections.clear();
		}

		bool Socket::isAccepting() const
		{
			return accepting;
		}

		void Socket::connectToOutgoingConnectionEstablishedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const std::shared_ptr<Connection>&)>& connectionAcceptedCallback)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			scopedConnection = outgoingConnectionEstablishedSignal.connect(connectionAcceptedCallback);
		}

		void Socket::connectToIncomingConnectionEstablishedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const std::shared_ptr<Connection>&)>& connectionAcceptedCallback)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			scopedConnection = incomingConnectionEstablishedSignal.connect(connectionAcceptedCallback);
		}

		void Socket::receiveHandler(std::vector<uint8_t>& data, const boost::asio::ip::udp::endpoint& senderEndpoint)
		{
			std::lock_guard<std::recursive_mutex> lock1(mutex);
			receivedPackets.emplace_back();
			receivedPackets.back().senderEndpoint = senderEndpoint;
			receivedPackets.back().data.swap(data);
		}

		bool Socket::open()
		{
			return sharedImpl->open();
		}

		void Socket::close()
		{
			sharedImpl->close();
		}

		bool Socket::bind(const Port& port)
		{
			return sharedImpl->open();
		}

		bool Socket::isOpen() const
		{
			return sharedImpl->isOpen();
		}

		Port Socket::getLocalPort() const
		{
			return sharedImpl->getLocalPort();
		}

		boost::asio::ip::udp::endpoint Socket::getLocalEndpoint() const
		{
			return sharedImpl->getLocalEndpoint();
		}

		size_t Socket::getSentBytes() const
		{
			return sharedImpl->getSentBytes();
		}

		size_t Socket::getReceivedBytes() const
		{
			return sharedImpl->getReceivedBytes();
		}

		void Socket::setDebugLogLevel(const int level)
		{
			sharedImpl->setDebugLogLevel(level);
		}

		int Socket::getDebugLogLevel() const
		{
			return sharedImpl->getDebugLogLevel();
		}
	}
}
