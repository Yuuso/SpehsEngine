#pragma once

#include "boost/asio/io_service.hpp"


namespace se
{
	namespace net
	{
		class IOService;
		boost::asio::io_service& getImplementationRef(IOService &ioService);
	}
}