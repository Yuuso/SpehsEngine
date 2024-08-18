#pragma once

#include "SpehsEngine/Net/Port.h"


namespace se
{
	namespace net
	{
		struct ISocketTCP;
		class Acceptor
		{
		public:
			friend struct ISocketTCP;
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