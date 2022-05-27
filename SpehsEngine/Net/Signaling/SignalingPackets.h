#pragma once

#include "SpehsEngine/Net/NetIdentity.h"
#include <vector>


namespace se
{
	namespace net
	{
		struct SignalingEntryPacket
		{
			void write(WriteBuffer& writeBuffer) const;
			bool read(ReadBuffer& readBuffer);

			NetIdentity netIdentity;
		};

		struct SignalingDataPacket
		{
			void write(WriteBuffer& writeBuffer) const;
			bool read(ReadBuffer& readBuffer);

			NetIdentity netIdentity;
			std::vector<uint8_t> data;
		};
	}
}
