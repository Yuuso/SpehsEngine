#pragma once

#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StrongInt.h"


namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	namespace net
	{
		SE_STRONG_INT(uint32_t, ProtocolId, 0u);
		SE_STRONG_INT(uint32_t, ConnectionId, 0u);

		struct PacketHeader
		{
			enum class ControlBit : uint8_t
			{
				UserData = 1 << 0,
				Reliable = 1 << 1,
				EndOfPayload = 1 << 2,
				AcknowledgePacket = 1 << 3,

				// Automatically managed on read/write
				EnableStreamOffset64Bit = 1 << 4,
				EnablePayloadSizeBit1 = 1 << 5,
				EnablePayloadSizeBit2 = 1 << 6,
				EnableReceivedPayloadSize16Bit = 1 << 7,
			};

			void write(se::WriteBuffer& writeBuffer, const std::string& debugName) const;
			bool read(se::ReadBuffer& readBuffer, const std::string& debugName);

			ProtocolId protocolId;
			ConnectionId connectionId;
			ControlBit controlBits = ControlBit(0);
			uint64_t streamOffset = 0u;
			uint64_t payloadTotalSize = 0u;
			uint16_t receivedPayloadSize = 0u;
		};

		struct ConnectPacket
		{
			void write(se::WriteBuffer& writeBuffer) const;
			bool read(se::ReadBuffer& readBuffer);

			std::string debugName;
			ConnectionId connectionId;
		};

		struct DisconnectPacket
		{
			void write(se::WriteBuffer& writeBuffer) const;
			bool read(se::ReadBuffer& readBuffer);
		};

		struct HeartbeatPacket
		{
			void write(se::WriteBuffer& writeBuffer) const;
			bool read(se::ReadBuffer& readBuffer);
		};

		struct PathMTUDiscoveryPacket
		{
			void write(se::WriteBuffer& writeBuffer) const;
			bool read(se::ReadBuffer& readBuffer);

			std::vector<uint8_t> payload;
		};
	}
}
