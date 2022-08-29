#include "stdafx.h"
#include "SpehsEngine/Net/Signaling/SignalingPackets.h"

#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"


namespace se
{
	namespace net
	{
		void SignalingEntryPacket::write(WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, netIdentity);
		}

		bool SignalingEntryPacket::read(ReadBuffer& readBuffer)
		{
			se_read(readBuffer, netIdentity);
			se_assert(readBuffer.getOffset() == readBuffer.getSize());
			return true;
		}

		void SignalingDataPacket::write(WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, netIdentity);
			se_write(writeBuffer, data);
		}

		bool SignalingDataPacket::read(ReadBuffer& readBuffer)
		{
			se_read(readBuffer, netIdentity);
			se_read(readBuffer, data);
			return true;
		}
	}
}
