#include "stdafx.h"
#include "SpehsEngine/Net/IOService.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Thread.h"
#include <boost/bind.hpp>


namespace se
{
	namespace net
	{
		IOService::IOService()
			: io_service()
			, thread(boost::bind(&IOService::run, this))
		{

		}

		IOService::~IOService()
		{
			try
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				io_service.stop();
				stop = true;
			}
			catch (std::exception& e)
			{
				log::info(e.what());
			}
			thread.join();
		}

		void IOService::run()
		{
			setThreadName("IOService::run()");
			boost::system::error_code error;
			while (true)
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				io_service.run_one(error);
				if (stop)
				{
					break;
				}
				if (error)
				{
					se::log::info("IOService: failed to run_one(). Boost asio error: " + error.message());
				}
			}
		}

		boost::asio::io_service& IOService::getImplementationRef()
		{
			return io_service;
		}
	}
}