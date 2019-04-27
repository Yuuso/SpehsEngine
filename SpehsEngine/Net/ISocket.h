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
			
			/* Returns the total number of sent bytes. Does not account bytes from the IP implementation. */
			virtual size_t getSentBytes() const = 0;

			/* Returns the total number of received bytes. Does not account bytes from the IP implementation. */
			virtual size_t getReceivedBytes() const = 0;
		};
	}
}
