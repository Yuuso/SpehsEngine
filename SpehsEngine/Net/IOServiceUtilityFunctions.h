#pragma once


namespace se
{
	namespace net
	{
		boost::asio::io_context& getImplementationRef(IOService &ioService);
	}
}