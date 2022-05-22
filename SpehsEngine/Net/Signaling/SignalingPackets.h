#pragma once

#include <string>
#include <vector>


namespace se
{
	namespace net
	{
		struct SignalingEntryPacket
		{
			void write(WriteBuffer& writeBuffer) const;
			bool read(ReadBuffer& readBuffer);

			std::string identity;
		};

		struct SignalingDataPacket
		{
			void write(WriteBuffer& writeBuffer) const;
			bool read(ReadBuffer& readBuffer);

			std::string identity;
			std::vector<uint8_t> data;
		};
	}
}
