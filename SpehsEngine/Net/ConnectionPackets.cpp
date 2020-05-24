#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionPackets.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#pragma optimize("", off) // nocommit


namespace se
{
	namespace net
	{
		const time::Time reliableHeartbeatInterval = time::fromSeconds(1.0f);
		const time::Time timeoutTime = reliableHeartbeatInterval * 5;

		void PacketHeader::write(se::WriteBuffer& writeBuffer, const std::string& debugName) const
		{
			const bool reliable = checkBit<ControlBit>(controlBits, ControlBit::Reliable);
			const bool acknowledgePacket = checkBit<ControlBit>(controlBits, ControlBit::AcknowledgePacket);
			ControlBit trueControlBits = controlBits;

			// Stream offset
			const bool enableStreamOffset32Bit = reliable || acknowledgePacket;
			const bool enableStreamOffset64Bit = enableStreamOffset32Bit && streamOffset > std::numeric_limits<uint32_t>::max();
			if (enableStreamOffset64Bit)
			{
				enableBit(trueControlBits, ControlBit::EnableStreamOffset64Bit);
			}
			else
			{
				disableBit(trueControlBits, ControlBit::EnableStreamOffset64Bit);
			}

			// Payload total size
			const bool enablePayloadTotalSize8Bit = reliable;
			const bool enablePayloadTotalSize16Bit = enablePayloadTotalSize8Bit && payloadTotalSize > std::numeric_limits<uint8_t>::max();
			const bool enablePayloadTotalSize32Bit = enablePayloadTotalSize16Bit && payloadTotalSize > std::numeric_limits<uint16_t>::max();
			const bool enablePayloadTotalSize64Bit = enablePayloadTotalSize32Bit && payloadTotalSize > std::numeric_limits<uint32_t>::max();
			if (enablePayloadTotalSize64Bit)
			{
				enableBit(trueControlBits, ControlBit::EnablePayloadSizeBit1);
				enableBit(trueControlBits, ControlBit::EnablePayloadSizeBit2);
			}
			else if (enablePayloadTotalSize32Bit)
			{
				disableBit(trueControlBits, ControlBit::EnablePayloadSizeBit1);
				enableBit(trueControlBits, ControlBit::EnablePayloadSizeBit2);
			}
			else if (enablePayloadTotalSize16Bit)
			{
				enableBit(trueControlBits, ControlBit::EnablePayloadSizeBit1);
				disableBit(trueControlBits, ControlBit::EnablePayloadSizeBit2);
			}
			else
			{
				disableBit(trueControlBits, ControlBit::EnablePayloadSizeBit1);
				disableBit(trueControlBits, ControlBit::EnablePayloadSizeBit2);
			}

			// Received payload size
			const bool enableReceivedPayloadSize8Bit = acknowledgePacket;
			const bool enableReceivedPayloadSize16Bit = enableReceivedPayloadSize8Bit && receivedPayloadSize > std::numeric_limits<uint8_t>::max();
			if (enableReceivedPayloadSize16Bit)
			{
				enableBit(trueControlBits, ControlBit::EnableReceivedPayloadSize16Bit);
			}
			else
			{
				disableBit(trueControlBits, ControlBit::EnableReceivedPayloadSize16Bit);
			}

			se_write(writeBuffer, protocolId);
			se_write(writeBuffer, connectionId);
			se_write(writeBuffer, trueControlBits);

			if (enableStreamOffset64Bit)
			{
				se_write(writeBuffer, streamOffset);
			}
			else if (enableStreamOffset32Bit)
			{
				const uint32_t streamOffset32 = uint32_t(streamOffset);
				se_write(writeBuffer, streamOffset32);
			}

			if (enablePayloadTotalSize64Bit)
			{
				se_write(writeBuffer, payloadTotalSize);
			}
			else if (enablePayloadTotalSize32Bit)
			{
				const uint32_t payloadSize32 = uint32_t(payloadTotalSize);
				se_write(writeBuffer, payloadSize32);
			}
			else if (enablePayloadTotalSize16Bit)
			{
				const uint16_t payloadSize16 = uint16_t(payloadTotalSize);
				se_write(writeBuffer, payloadSize16);
			}
			else if (enablePayloadTotalSize8Bit)
			{
				const uint8_t payloadSize8 = uint8_t(payloadTotalSize);
				se_write(writeBuffer, payloadSize8);
			}

			if (enableReceivedPayloadSize16Bit)
			{
				se_write(writeBuffer, receivedPayloadSize);
			}
			else if (enableReceivedPayloadSize8Bit)
			{
				const uint8_t receivedPayloadSize8 = uint8_t(receivedPayloadSize);
				se_write(writeBuffer, receivedPayloadSize8);
			}
		}

		bool PacketHeader::read(se::ReadBuffer& readBuffer, const std::string& debugName)
		{
			se_read(readBuffer, protocolId);
			se_read(readBuffer, connectionId);
			se_read(readBuffer, controlBits);
			const bool reliable = checkBit<ControlBit>(controlBits, ControlBit::Reliable);
			const bool acknowledgePacket = checkBit<ControlBit>(controlBits, ControlBit::AcknowledgePacket);

			// Stream offset
			if (reliable || acknowledgePacket)
			{
				if (checkBit<ControlBit>(controlBits, ControlBit::EnableStreamOffset64Bit))
				{
					se_read(readBuffer, streamOffset);
				}
				else
				{
					uint32_t streamOffset32 = 0u;
					se_read(readBuffer, streamOffset32);
					streamOffset = uint64_t(streamOffset32);
				}
			}

			// Payload total size
			if (reliable)
			{
				const bool bit1 = checkBit<ControlBit>(controlBits, ControlBit::EnablePayloadSizeBit1);
				const bool bit2 = checkBit<ControlBit>(controlBits, ControlBit::EnablePayloadSizeBit2);
				if (bit1)
				{
					if (bit2)
					{
						se_read(readBuffer, payloadTotalSize);
					}
					else
					{
						uint16_t payloadTotalSize16 = 0u;
						se_read(readBuffer, payloadTotalSize16);
						payloadTotalSize = uint64_t(payloadTotalSize16);
					}
				}
				else
				{
					if (bit2)
					{
						uint32_t payloadTotalSize32 = 0u;
						se_read(readBuffer, payloadTotalSize32);
						payloadTotalSize = uint64_t(payloadTotalSize32);
					}
					else
					{
						uint8_t payloadTotalSize8 = 0u;
						se_read(readBuffer, payloadTotalSize8);
						payloadTotalSize = uint64_t(payloadTotalSize8);
					}
				}
			}

			// Received payload size
			if (acknowledgePacket)
			{
				se_read(readBuffer, receivedPayloadSize);
				if (checkBit<ControlBit>(controlBits, ControlBit::EnableReceivedPayloadSize16Bit))
				{
					se_read(readBuffer, receivedPayloadSize);
				}
				else
				{
					uint8_t receivedPayloadSize8 = 0u;
					se_read(readBuffer, receivedPayloadSize8);
					receivedPayloadSize = uint16_t(receivedPayloadSize8);
				}
			}

			return true;
		}

		void ConnectPacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, connectionId);
			se_write(writeBuffer, debugName);
		}

		bool ConnectPacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, connectionId);
			se_read(readBuffer, debugName);
			return true;
		}

		void DisconnectPacket::write(se::WriteBuffer& writeBuffer) const
		{
		}

		bool DisconnectPacket::read(se::ReadBuffer& readBuffer)
		{
			return true;
		}

		void HeartbeatPacket::write(se::WriteBuffer& writeBuffer) const
		{
		}

		bool HeartbeatPacket::read(se::ReadBuffer& readBuffer)
		{
			return true;
		}

		void PathMTUDiscoveryPacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, payload);
		}
		bool PathMTUDiscoveryPacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, payload);
			return true;
		}
	}
}
