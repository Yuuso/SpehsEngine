#pragma once
#include <boost/asio/io_service.hpp>
#include <thread>
#include <mutex>

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

			std::recursive_mutex mutex;
			boost::asio::io_service io_service;
			bool stop = false;
			std::thread thread;
		};
	}
}