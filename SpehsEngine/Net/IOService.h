#pragma once
#include <boost/asio/io_service.hpp>
#include <thread>

namespace se
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

		boost::asio::io_service io_service;
		boost::asio::io_service::work work;
		std::thread thread;
	};
}