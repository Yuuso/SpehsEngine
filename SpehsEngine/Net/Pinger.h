#pragma once
#include "boost/asio.hpp"
#include "SpehsEngine/Net/HeaderICMP.h"
#include "SpehsEngine/Net/HeaderIPv4.h"

namespace posix_time = boost::posix_time;

namespace se
{
	namespace net
	{
		class IOService;

		/*
			Copy paste code from asio's pinger example.
		*/
		class Pinger
		{
		public:
			Pinger(IOService& ioService, const char* destinationAddress);

		private:

			void startSend();
			void handleTimeout();
			void startReceive();
			void handleReceive(const size_t length);
			
			boost::asio::ip::icmp::resolver resolver;
			boost::asio::ip::icmp::endpoint destination;
			boost::asio::ip::icmp::socket socket;
			boost::asio::deadline_timer timer;
			unsigned short sequenceNumber = 0;
			posix_time::ptime timeSent;
			boost::asio::streambuf replyBuffer;
			std::size_t numReplies = 0;
		};
	}
}
