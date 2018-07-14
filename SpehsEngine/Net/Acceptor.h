#pragma once
#include <thread>
#include <stdint.h>
#include <boost/asio.hpp>
#include "SpehsEngine/Net/Port.h"

namespace se
{
	namespace net
	{
		class SocketTCP;
		class Acceptor
		{
		public:
			friend class SocketTCP;
		public:
			Acceptor();
			~Acceptor();

			bool open(const Port& _port);

		private:
			Port port;
			boost::asio::io_service ioService;
			boost::asio::io_service::work ioServiceWork;
			std::thread ioServiceThread;
			boost::asio::ip::tcp::acceptor acceptor;
		};
	}
}