#include "stdafx.h"
#include "SpehsEngine/Net/IOService.h"

#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Net/IOServiceUtilityFunctions.h"
#include "boost/asio/io_service.hpp"
#include "boost/bind.hpp"
#include <thread>


namespace se
{
	namespace net
	{
		struct IOService::Impl
		{
			Impl()
				: ioService()
				, ioServiceWork(ioService)
				, thread(std::bind(&Impl::run, this))
			{
			}

			~Impl()
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

			void run()
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

			static boost::asio::io_service& getImplementationRef(IOService& ioService)
			{
				return ioService.impl->ioService;
			}

			boost::asio::io_service ioService;
			std::optional<boost::asio::io_service::work> ioServiceWork;
			std::thread thread;
		};

		boost::asio::io_context& getImplementationRef(IOService& ioService)
		{
			return IOService::Impl::getImplementationRef(ioService);
		}

		IOService::IOService()
			: impl(new Impl())
		{
		}

		IOService::~IOService()
		{
		}
	}
}