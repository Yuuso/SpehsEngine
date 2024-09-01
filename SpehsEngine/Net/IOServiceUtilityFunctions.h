#pragma once

namespace boost
{
	namespace asio
	{
		class io_context;
	}
}


namespace se
{
	namespace net
	{
		class IOService;
		boost::asio::io_context& getImplementationRef(IOService &ioService);
	}
}