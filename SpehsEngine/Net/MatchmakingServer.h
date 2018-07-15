#pragma once
#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/Port.h"
#include "SpehsEngine/Net/SocketTCP.h"

namespace se
{
	namespace net
	{
		class SocketTCP;
		class MatchmakingServer
		{
		public:
			MatchmakingServer(const Port port = 41623);

			void update();
			void stopAccepting();

			void setPort(const Port& port);
			Port getPort() const;

			size_t getConnectedSocketCount() const;
			std::vector<SocketTCP*> getConnectedSockets();

		private:
			void onAcceptCallback(SocketTCP& socketTCP);

			IOService ioService;
			Acceptor acceptor;
			std::vector<std::unique_ptr<SocketTCP>> sockets;
			Port port;
		};
	}
}