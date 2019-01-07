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

		}

		void MultiConnectionAcceptorTCP::update()
		{
			if (awaitingAcceptingSocket)
			{
				awaitingAcceptingSocket->update();
				if (awaitingAcceptingSocket->getAcceptingState() == SocketTCP::AcceptingState::idle)
				{
					if (accepting)
					{
						awaitingAcceptingSocket->startAccepting(port, std::bind(&MultiConnectionAcceptorTCP::onAcceptCallback, this, std::placeholders::_1));
					}
				}
				else if (awaitingAcceptingSocket->getAcceptingState() == SocketTCP::AcceptingState::establishingConnection)
				{
					busyAcceptingSockets.push_back(std::unique_ptr<SocketTCP>(awaitingAcceptingSocket.release()));
				}
			}
			else
			{
				awaitingAcceptingSocket.reset(new se::net::SocketTCP(ioService));
			}
			for (size_t i = 0; i < busyAcceptingSockets.size(); i++)
			{
				busyAcceptingSockets[i]->update();
			}
		}

		void MultiConnectionAcceptorTCP::startAccepting()
		{
			accepting = true;
			log::info("MultiConnectionAcceptorTCP: started accepting connections");
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
			}
			for (size_t i = 0; i < busyAcceptingSockets.size(); i++)
			{
				if (busyAcceptingSockets[i]->getAcceptingState() != SocketTCP::AcceptingState::idle)
				{
					busyAcceptingSockets[i]->stopAccepting();
				}
			}
			log::info("MultiConnectionAcceptorTCP: stopped accepting connections");
		}

		void MultiConnectionAcceptorTCP::onAcceptCallback(SocketTCP& socketTCP)
		{
			if (&socketTCP == awaitingAcceptingSocket.get())
			{
				if (socketTCP.isConnected())
				{
					connectedSockets.push_back(std::unique_ptr<SocketTCP>(awaitingAcceptingSocket.release()));
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