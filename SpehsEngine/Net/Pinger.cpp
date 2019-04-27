#include "stdafx.h"
#include "SpehsEngine/Net/Pinger.h"

#include "SpehsEngine/Net/IOService.h"
#include "boost/bind.hpp"
#include <istream>
#include <iostream>
#include <ostream>

namespace se
{
	namespace net
	{
		static unsigned short getIdentifier()
		{
#if defined(BOOST_WINDOWS)
			return static_cast<unsigned short>(::GetCurrentProcessId());
#else
			return static_cast<unsigned short>(::getpid());
#endif
		}

		Pinger::Pinger(IOService& ioService, const char* destinationAddress)
			: resolver(ioService.getImplementationRef())
			, socket(ioService.getImplementationRef(), boost::asio::ip::icmp::endpoint(boost::asio::ip::icmp::v4(), 0))
			, timer(ioService.getImplementationRef())
		{
			boost::asio::ip::icmp::resolver::query query(boost::asio::ip::icmp::v4(), destinationAddress, "");
			destination = *resolver.resolve(query);

			startSend();
			startReceive();
		}

		void Pinger::startSend()
		{
			std::string message("\"Hello!\" from Asio ping.");

			// Create an ICMP header for an echo request.
			HeaderICMP header;
			header.setType(HeaderICMP::echo_request);
			header.setCode(0);
			header.setIdentifier(getIdentifier());
			header.setSequenceNumber(++sequenceNumber);
			computeChecksum(header, message.begin(), message.end());

			// Encode the request packet.
			boost::asio::streambuf requestBuffer;
			std::ostream os(&requestBuffer);
			os << header << message;

			// Send the request.
			timeSent = posix_time::microsec_clock::universal_time();
			socket.send_to(requestBuffer.data(), destination);

			// Wait up to five seconds for a reply.
			numReplies = 0;
			timer.expires_at(timeSent + posix_time::seconds(5));
			timer.async_wait(boost::bind(&Pinger::handleTimeout, this));
		}

		void Pinger::handleTimeout()
		{
			if (numReplies == 0)
			{
				std::cout << "Request timed out" << std::endl;
			}

			// Requests must be sent no less than one second apart.
			timer.expires_at(timeSent + posix_time::seconds(1));
			timer.async_wait(boost::bind(&Pinger::startSend, this));
		}

		void Pinger::startReceive()
		{
			// Discard any data already in the buffer.
			replyBuffer.consume(replyBuffer.size());

			// Wait for a reply. We prepare the buffer to receive up to 64KB.
			socket.async_receive(replyBuffer.prepare(65536), boost::bind(&Pinger::handleReceive, this, _2));
		}

		void Pinger::handleReceive(const size_t length)
		{
			// The actual number of bytes received is committed to the buffer so that we
			// can extract it using a std::istream object.
			replyBuffer.commit(length);

			// Decode the reply packet.
			std::istream is(&replyBuffer);
			HeaderIPv4 headerIPv4;
			HeaderICMP headerICMP;
			is >> headerIPv4 >> headerICMP;

			// We can receive all ICMP packets received by the host, so we need to
			// filter out only the echo replies that match the our identifier and
			// expected sequence number.
			if (is && headerICMP.getType() == HeaderICMP::echo_reply
				&& headerICMP.getIdentifier() == getIdentifier()
				&& headerICMP.getSequenceNumber() == sequenceNumber)
			{
				// If this is the first reply, interrupt the five second timeout.
				if (numReplies++ == 0)
					timer.cancel();

				// Print out some information about the reply packet.
				posix_time::ptime now = posix_time::microsec_clock::universal_time();
				std::cout << length - headerIPv4.getHeaderLength()
					<< " bytes from " << headerIPv4.getSourceAddress()
					<< ": icmp_seq=" << headerICMP.getSequenceNumber()
					<< ", ttl=" << headerIPv4.getTimeToLive()
					<< ", time=" << (now - timeSent).total_milliseconds() << " ms"
					<< std::endl;
			}

			startReceive();
		}
	}
}
