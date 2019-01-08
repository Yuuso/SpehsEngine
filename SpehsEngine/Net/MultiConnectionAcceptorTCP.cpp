#include "stdafx.h"
#include "SpehsEngine/Net/MultiConnectionAcceptorTCP.h"

#include <SpehsEngine/Core/Log.h>

namespace se
{
	namespace net
	{
		MultiConnectionAcceptorTCP::MultiConnectionAcceptorTCP(IOService& _ioService, const Port _port)
			: ioService(_ioService)
			, port(_port)
		{
			awaitingAcceptingSocket.reset(new se::net::SocketTCP(ioService));
			awaitingAcceptingSocket->startAccepting(port, std::bind(&MultiConnectionAcceptorTCP::onAcceptCallback, this, std::placeholders::_1));
		}

		void MultiConnectionAcceptorTCP::update()
		{
			if (awaitingAcceptingSocket)
			{
				awaitingAcceptingSocket->update();
				se_assert(awaitingAcceptingSocket);
				if (awaitingAcceptingSocket->getAcceptingState() == SocketTCP::AcceptingState::idle)
				{
					if (accepting)
					{
						awaitingAcceptingSocket->startAccepting(port, std::bind(&MultiConnectionAcceptorTCP::onAcceptCallback, this, std::placeholders::_1));
					}
				}
				else if (awaitingAcceptingSocket->getAcceptingState() == SocketTCP::AcceptingState::establishingConnection)
				{
					//Push to busy accepting sockets so that we can start taking in another connection
					busyAcceptingSockets.push_back(std::unique_ptr<SocketTCP>(awaitingAcceptingSocket.release()));
					awaitingAcceptingSocket.reset(new se::net::SocketTCP(ioService));
					awaitingAcceptingSocket->startAccepting(port, std::bind(&MultiConnectionAcceptorTCP::onAcceptCallback, this, std::placeholders::_1));
					awaitingAcceptingSocket->update();
				}
			}
			else if (accepting)
			{
				awaitingAcceptingSocket.reset(new se::net::SocketTCP(ioService));
				awaitingAcceptingSocket->startAccepting(port, std::bind(&MultiConnectionAcceptorTCP::onAcceptCallback, this, std::placeholders::_1));
				awaitingAcceptingSocket->update();
			}

			//NOTE: updating the socket might result in accepting, thus releasing the pointer. Do this after the above...
			se_assert(awaitingAcceptingSocket);
			awaitingAcceptingSocket->update();

			for (size_t i = 0; i < busyAcceptingSockets.size(); i++)
			{
				busyAcceptingSockets[i]->update();
			}
		}

		void MultiConnectionAcceptorTCP::startAccepting()
		{
			accepting = true;
			log::info("MultiConnectionAcceptorTCP: started accepting connections");
			awaitingAcceptingSocket.reset(new se::net::SocketTCP(ioService));
			awaitingAcceptingSocket->startAccepting(port, std::bind(&MultiConnectionAcceptorTCP::onAcceptCallback, this, std::placeholders::_1));
		}

		void MultiConnectionAcceptorTCP::stopAccepting()
		{
			accepting = false;
			if (awaitingAcceptingSocket)
			{
				if (awaitingAcceptingSocket->getAcceptingState() != SocketTCP::AcceptingState::idle)
				{
					awaitingAcceptingSocket->stopAccepting();
				}
				awaitingAcceptingSocket.reset();
			}
			for (size_t i = 0; i < busyAcceptingSockets.size(); i++)
			{
				if (busyAcceptingSockets[i]->getAcceptingState() != SocketTCP::AcceptingState::idle)
				{
					busyAcceptingSockets[i]->stopAccepting();
				}
			}
			busyAcceptingSockets.clear();
			log::info("MultiConnectionAcceptorTCP: stopped accepting connections");
		}

		void MultiConnectionAcceptorTCP::onAcceptCallback(SocketTCP& socketTCP)
		{
			if (&socketTCP == awaitingAcceptingSocket.get())
			{
				if (socketTCP.isConnected())
				{
					connectedSockets.push_back(std::unique_ptr<SocketTCP>(awaitingAcceptingSocket.release()));
					awaitingAcceptingSocket.reset(new se::net::SocketTCP(ioService));
					awaitingAcceptingSocket->startAccepting(port, std::bind(&MultiConnectionAcceptorTCP::onAcceptCallback, this, std::placeholders::_1));
				}
				else
				{
					se::log::info("Failed to accept client");
				}
			}
			else
			{
				for (size_t i = 0; i < busyAcceptingSockets.size(); i++)
				{
					if (&socketTCP == busyAcceptingSockets[i].get())
					{
						if (socketTCP.isConnected())
						{
							connectedSockets.push_back(std::unique_ptr<SocketTCP>(busyAcceptingSockets[i].release()));
						}
						else
						{
							se::log::info("Failed to accept client");
						}
						busyAcceptingSockets[i].reset(busyAcceptingSockets.back().release());
						busyAcceptingSockets.pop_back();
						return;
					}
				}
				se_assert(false && "Socket not found.");
			}
		}

		const std::vector<std::unique_ptr<SocketTCP>>& MultiConnectionAcceptorTCP::getConnectedSockets() const
		{
			return connectedSockets;
		}

		void MultiConnectionAcceptorTCP::releaseConnectedSockets(std::vector<std::unique_ptr<SocketTCP>>& deposit)
		{
			for (size_t i = 0; i < connectedSockets.size(); i++)
			{
				deposit.push_back(std::unique_ptr<SocketTCP>(connectedSockets[i].release()));
			}
			connectedSockets.clear();
		}

		void MultiConnectionAcceptorTCP::setPort(const Port& _port)
		{
			port = _port;
			if (awaitingAcceptingSocket)
			{
				awaitingAcceptingSocket->stopAccepting();
			}
		}

		Port MultiConnectionAcceptorTCP::getPort() const
		{
			return port;
		}
	}
}