#include "stdafx.h"
#include "SpehsEngine/Net/MatchmakingServer.h"

#include <SpehsEngine/Core/Log.h>



namespace se
{
	namespace net
	{
		MatchmakingServer::MatchmakingServer(IOService& _ioService, const Port _port)
			: ioService(_ioService)
			, port(_port)
		{

		}

		void MatchmakingServer::update()
		{
			size_t acceptingClientCount = 0;
			for (size_t i = 0; i < sockets.size(); i++)
			{
				sockets[i]->update();
				if (sockets[i]->isAccepting())
				{
					acceptingClientCount++;
				}
				else if (!sockets[i]->isConnected())
				{
					sockets.erase(sockets.begin() + i);
					i--;
				}
			}
			if (acceptingClientCount == 0)
			{
				log::info("MatchMakingServer: Awaiting for a connecting client");
				sockets.push_back(std::unique_ptr<SocketTCP>(new SocketTCP(ioService)));
				sockets.back()->startAccepting(port, std::bind(&MatchmakingServer::onAcceptCallback, this, std::placeholders::_1));
			}
			else
			{
				se_assert(acceptingClientCount == 1);
			}
		}

		void MatchmakingServer::stopAccepting()
		{
			for (size_t i = 0; i < sockets.size(); i++)
			{
				if (sockets[i]->isAccepting())
					sockets[i]->stopAccepting();
			}
			log::info("MatchMakingServer: stopped accepting clients");
		}

		void MatchmakingServer::onAcceptCallback(SocketTCP& socketTCP)
		{
			for (size_t i = 0; i < sockets.size(); i++)
			{
				if (sockets[i].get() == &socketTCP)
				{
					if (socketTCP.isConnected())
					{
						log::info("MatchMakingServer: Client successfully connected from " + socketTCP.getRemoteEndpoint().toString());
					}
					else
					{
						log::info("MatchMakingServer: Failed to accept client");
					}
					return;
				}
			}
		}

		size_t MatchmakingServer::getConnectedSocketCount() const
		{
			size_t count = 0;
			for (size_t i = 0; i < sockets.size(); i++)
			{
				if (sockets[i]->isConnected())
					count++;
			}
			return count;
		}

		std::vector<SocketTCP*> MatchmakingServer::getConnectedSockets()
		{
			std::vector<SocketTCP*> connectedSockets;
			for (size_t i = 0; i < sockets.size(); i++)
			{
				if (sockets[i]->isConnected())
					connectedSockets.push_back(sockets[i].get());
			}
			return connectedSockets;
		}

		void MatchmakingServer::setPort(const Port& _port)
		{
			port = _port;
			for (size_t i = 0; i < sockets.size(); i++)
			{
				if (sockets[i]->isAccepting())
					sockets[i]->stopAccepting();
			}
		}

		Port MatchmakingServer::getPort() const
		{
			return port;
		}
	}
}