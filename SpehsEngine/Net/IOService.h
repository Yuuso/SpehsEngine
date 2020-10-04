#pragma once

#include "boost/asio/io_service.hpp"
#include <optional>
#include <thread>


namespace se
{
	namespace net
	{
		/*Upon creation, starts to run an IO service in a separate thread*/
		class IOService
		{
		public:
			IOService();
			~IOService();

			boost::asio::io_service& getImplementationRef();

		private:
			void run();

			boost::asio::io_service ioService;
			std::optional<boost::asio::io_service::work> ioServiceWork;
			std::thread thread;
		};
	}
}