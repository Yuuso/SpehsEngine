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
		const ProtocolId PacketHeader::spehsProtocolId(1397770323);

		void PacketHeader::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, protocolId);
			se_write(writeBuffer, connectionId);
			se_write(writeBuffer, packetType);
		}
		bool PacketHeader::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, protocolId);
			se_read(readBuffer, connectionId);
			se_read(readBuffer, packetType);
			return true;
		}

		void ReliableFragmentPacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_assert(payload.size() <= size_t(std::numeric_limits<uint16_t>::max()) && "Payload is too big to fit a single fragment.");
			writeToBuffer<uint8_t, uint16_t>(writeBuffer, payload);
			se_write(writeBuffer, streamOffset);
			se_write(writeBuffer, payloadTotalSize);
			se_write(writeBuffer, endOfPayload);
		}
		bool ReliableFragmentPacket::read(se::ReadBuffer& readBuffer)
		{
			readFromBuffer<uint8_t, uint16_t>(readBuffer, payload);
			se_read(readBuffer, streamOffset);
			se_read(readBuffer, payloadTotalSize);
			se_read(readBuffer, endOfPayload);
			return true;
		}

		void AcknowledgePacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, streamOffset);
			se_write(writeBuffer, payloadSize);
		}
		bool AcknowledgePacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, streamOffset);
			se_read(readBuffer, payloadSize);
			return true;
		}

		void ConnectPacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, connectionId);
			writeToBuffer<uint8_t>(writeBuffer, debugName);
		}
		bool ConnectPacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, connectionId);
			readFromBuffer<uint8_t>(readBuffer, debugName);
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

		void PathMaximumSegmentSizeDiscoveryPacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, size);
			se_write(writeBuffer, extraPayload);
		}
		bool PathMaximumSegmentSizeDiscoveryPacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, size);
			se_read(readBuffer, extraPayload);
			return true;
		}

		void PathMaximumSegmentSizeAcknowledgePacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, size);
		}
		bool PathMaximumSegmentSizeAcknowledgePacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, size);
			return true;
		}

		void UserDataPacket::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, payload);
		}
		bool UserDataPacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, payload);
			return true;
		}
	}
}

/*

Optimize packet size

			// Stream offset
			const bool enableStreamOffset32Bit = reliableFragment || acknowledgePacket;
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
			const bool enablePayloadTotalSize8Bit = reliableFragment;
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



















			// Stream offset
			if (reliableFragment || acknowledgePacket)
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

*/