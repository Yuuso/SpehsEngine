#include "stdafx.h"
#include <boost/bind.hpp>
#include "SpehsEngine/Net/Acceptor.h"
#include "SpehsEngine/Net/SocketTCP.h"
#include "SpehsEngine/Core/Log.h"

namespace se
{
	namespace net
	{
		Acceptor::Acceptor()
			: ioServiceWork(ioService)
			, ioServiceThread(boost::bind(&boost::asio::io_service::run, &ioService))
			, acceptor(ioService)
		{
		}

		Acceptor::~Acceptor()
		{
			if (acceptor.is_open())
			{
				acceptor.cancel();
				acceptor.close();
			}
			ioService.stop();
			ioServiceThread.join();
		}

		bool Acceptor::open(const Port& _port)
		{
			port = _port;
			boost::system::error_code error;
			const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);

			//Open acceptor
			acceptor.open(endpoint.protocol(), error);
			if (error)
			{
				log::error("Failed to open acceptor! Boost asio error: " + error.message());
				return false;
			}
			acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

			//Bind acceptor
			acceptor.bind(endpoint, error);
			if (error)
			{
				log::error("Failed to bind acceptor! Boost asio error: " + error.message());
				return false;
			}
			if (!acceptor.is_open())
			{
				log::error("Boost acceptor failed to open!");
				return false;
			}

			//Make acceptor listen
			acceptor.listen(boost::asio::socket_base::max_connections, error);
			if (error)
			{
				log::error("Failed to make acceptor listen! Boost asio error: " + error.message());
				return false;
			}

			return true;
		}
	}
}