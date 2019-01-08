#pragma once
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/Port.h"
#include "SpehsEngine/Net/SocketTCP.h"

namespace se
{
	namespace net
	{
		class SocketTCP;
		class MultiConnectionAcceptorTCP
		{
		public:
			MultiConnectionAcceptorTCP(IOService& ioService, const Port port = 41622);

			void update();

			void startAccepting();
			void stopAccepting();
			bool isAccepting() const { return accepting; }

			void setPort(const Port& port);
			Port getPort() const;

			const std::vector<std::unique_ptr<SocketTCP>>& getConnectedSockets() const;
			void releaseConnectedSockets(std::vector<std::unique_ptr<SocketTCP>>& deposit);

		private:
			
			void onAcceptCallback(SocketTCP& socketTCP);

			IOService& ioService;
			Port port;
			bool accepting = true;
			std::unique_ptr<se::net::SocketTCP> awaitingAcceptingSocket;
			std::vector<std::unique_ptr<se::net::SocketTCP>> busyAcceptingSockets;
			std::vector<std::unique_ptr<SocketTCP>> connectedSockets;
		};
	}
}
