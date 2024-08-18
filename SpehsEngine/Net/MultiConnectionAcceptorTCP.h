#pragma once
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Net/Port.h"

namespace se
{
	namespace net
	{
		struct ISocketTCP;

		class MultiConnectionAcceptorTCP
		{
		public:
			MultiConnectionAcceptorTCP(IOService& ioService, const Port port = 41622);
			~MultiConnectionAcceptorTCP();

			void update();

			void startAccepting();
			void stopAccepting();
			bool isAccepting() const { return accepting; }

			void setPort(const Port& port);
			Port getPort() const;

			const std::vector<std::unique_ptr<ISocketTCP>>& getConnectedSockets() const;
			void releaseConnectedSockets(std::vector<std::unique_ptr<ISocketTCP>>& deposit);

		private:
			
			void onAcceptCallback(ISocketTCP& socketTCP);

			IOService& ioService;
			Port port;
			bool accepting = true;
			std::unique_ptr<ISocketTCP> awaitingAcceptingSocket;
			std::vector<std::unique_ptr<ISocketTCP>> busyAcceptingSockets;
			std::vector<std::unique_ptr<ISocketTCP>> connectedSockets;
		};
	}
}
