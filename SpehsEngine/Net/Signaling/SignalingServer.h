#pragma once

#include "SpehsEngine/Net/Port.h"
#include <memory>


namespace se
{
	namespace net
	{
		class IOService;

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
