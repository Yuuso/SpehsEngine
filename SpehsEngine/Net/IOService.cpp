#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Core/Log.h"
#include <boost/bind.hpp>


namespace se
{
	IOService::IOService()
		: io_service()
		, work(io_service)
		, thread(boost::bind(&IOService::run, this))
	{		

	}
	
	IOService::~IOService()
	{
		try
		{
			io_service.stop();
		}
		catch (std::exception& e)
		{
			se::log::info(e.what());
		}
		thread.join();
	}

	void IOService::run()
	{
		try
		{
			io_service.run();
		}
		catch (std::exception& e)
		{
			se::log::info(e.what());
		}
	}

	boost::asio::io_service& IOService::getImplementationRef()
	{
		return io_service;
	}
}