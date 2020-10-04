#include "stdafx.h"
#include "SpehsEngine/Net/IOService.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Core/ScopedFrameLimiter.h"
#include <boost/bind.hpp>


namespace se
{
	namespace net
	{
		IOService::IOService()
			: ioService()
			, ioServiceWork(ioService)
			, thread(boost::bind(&IOService::run, this))
		{

		}

		IOService::~IOService()
		{
			try
			{
				ioServiceWork.reset(); // End work.
				ioService.stop();
			}
			catch (std::exception& e)
			{
				log::info(e.what());
			}
			thread.join();
		}

		void IOService::run()
		{
			/*
				NOTES:
				The 'ioServiceWork' member attribute prevents the run() method from running out of work and effectively blocking until destructor is called.
				Previously we used a while loop here and calling run_one() to execute ioService's handlers.
					-However, the previous while loop approach results in the application's cpu usage rising to 'very high'.
					-Limiting the loop's "frame time" by hand reduces network performance significantly.
					-One downside with using 'work' seems occasional spikes in network performance.
			*/
			setThreadName("IOService::run()");
			boost::system::error_code error;
			ioService.run(error);
			if (error)
			{
				se::log::info("IOService::run() error: " + error.message());
			}
		}

		boost::asio::io_service& IOService::getImplementationRef()
		{
			return ioService;
		}
	}
}