#pragma once

#include "SpehsEngine/Net/Port.h"


namespace se
{
	namespace net
	{
		class SignalingServer
		{
		public:

			SignalingServer(IOService& ioService, const Port& port);
			~SignalingServer();

			void update();

		private:
			struct Impl;
			std::unique_ptr<Impl> impl;
		};
	}
}
