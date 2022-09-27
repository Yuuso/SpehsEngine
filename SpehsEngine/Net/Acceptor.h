#pragma once

#include "SpehsEngine/Net/Port.h"


namespace se
{
	namespace net
	{
		class SocketTCP;
		class Acceptor
		{
		public:
			friend class SocketTCP;
		public:
			Acceptor();
			~Acceptor();

			bool open(const Port& _port);

		private:
			struct State;
			std::unique_ptr<State> state;
			Port port;
		};
	}
}