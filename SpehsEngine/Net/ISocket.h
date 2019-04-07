#pragma once
#include "SpehsEngine/Net/Port.h"

namespace se
{
	namespace net
	{
		struct Endpoint;

		class ISocket
		{
		public:

			virtual void update() = 0;

			virtual bool connect(const Endpoint& endpoint) = 0;

			virtual bool open() = 0;
			virtual bool bind(const Port& port) = 0;
			virtual void close() = 0;

			/* Returns the total number of sent bytes. Does not account bytes from the IP implementation. */
			virtual size_t getSentBytes() const = 0;

			/* Returns the total number of received bytes. Does not account bytes from the IP implementation. */
			virtual size_t getReceivedBytes() const = 0;

			virtual bool isOpen() const = 0;
			virtual Port getLocalPort() const = 0;
		};
	}
}
