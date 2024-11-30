#include "stdafx.h"
#include "SpehsEngine/Net/Acceptor.h"

#include "boost/asio/io_service.hpp"
#include "boost/bind.hpp"
#include "SpehsEngine/Core/Log.h"
#include <boost/asio/ip/tcp.hpp>
#include <thread>


namespace se
{
	namespace net
	{
		struct Acceptor::State
		{
			State()
				: ioServiceWork(ioService)
				, ioServiceThread(boost::bind(&boost::asio::io_service::run, &ioService))
				, acceptor(ioService)
			{}
			boost::asio::io_service ioService;
			boost::asio::io_service::work ioServiceWork;
			std::thread ioServiceThread;
			boost::asio::ip::tcp::acceptor acceptor;
		};

		Acceptor::Acceptor()
			: state(new State())
		{
		}

		Acceptor::~Acceptor()
		{
			if (state->acceptor.is_open())
			{
				state->acceptor.cancel();
				state->acceptor.close();
			}
			state->ioService.stop();
			state->ioServiceThread.join();
		}

		bool Acceptor::open(const Port& _port)
		{
			port = _port;
			boost::system::error_code error;
			const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

			//Open acceptor
			state->acceptor.open(endpoint.protocol(), error);
			if (error)
			{
				log::error("Failed to open acceptor! Boost asio error: " + error.message());
				return false;
			}
			state->acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

			//Bind acceptor
			state->acceptor.bind(endpoint, error);
			if (error)
			{
				log::error("Failed to bind acceptor! Boost asio error: " + error.message());
				return false;
			}
			if (!state->acceptor.is_open())
			{
				log::error("Boost acceptor failed to open!");
				return false;
			}

			//Make acceptor listen
			state->acceptor.listen(boost::asio::socket_base::max_connections, error);
			if (error)
			{
				log::error("Failed to make acceptor listen! Boost asio error: " + error.message());
				return false;
			}

			return true;
		}
	}
}