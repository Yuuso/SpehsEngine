#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionPackets.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/WriteBuffer.h"


namespace se
{
	namespace net
	{
		const ProtocolId PacketHeader::spehsProtocolId(0x53485053);

		uint16_t PacketHeader::getSize()
		{
			return sizeof(PacketHeader::protocolId) + sizeof(PacketHeader::connectionId) + sizeof(PacketHeader::packetType);
		}

		void PacketHeader::write(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, protocolId);
			se_write(writeBuffer, connectionId);
			se_write(writeBuffer, packetType);
			// NOTE: update ReliableFragmentPacket::getPacketPayloadHeaderSize()
		}
		bool PacketHeader::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, protocolId);
			se_read(readBuffer, connectionId);
			se_read(readBuffer, packetType);
			return true;
		}

		uint16_t ReliableFragmentPacket::getHeaderSize()
		{
			return sizeof(streamOffset) + sizeof(payloadTotalSize) + sizeof(endOfPayload);
		}

		void ReliableFragmentPacket::writeHeader(se::WriteBuffer& writeBuffer) const
		{
			se_write(writeBuffer, streamOffset);
			se_write(writeBuffer, payloadTotalSize);
			se_write(writeBuffer, endOfPayload);
			// NOTE: remember to update getPacketPayloadHeaderSize()
		}
		bool ReliableFragmentPacket::readHeader(se::ReadBuffer& readBuffer)
		{
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
			se_write(writeBuffer, expectingResponse);
		}
		bool DisconnectPacket::read(se::ReadBuffer& readBuffer)
		{
			se_read(readBuffer, expectingResponse);
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

		bool UserDataPacket::read(se::ReadBuffer& readBuffer)
		{
			readFromBuffer<uint8_t, PayloadSizeType>(readBuffer, payload);
			return true;
		}
	}
}
