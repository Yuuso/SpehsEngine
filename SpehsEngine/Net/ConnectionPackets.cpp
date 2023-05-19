#include "stdafx.h"
//#include "SpehsEngine/Net/ConnectionPackets.h"
//
//
//namespace se
//{
//	namespace net
//	{
//		const ProtocolId PacketHeader::spehsProtocolId(0x53485053);
//
//		uint16_t PacketHeader::getSize()
//		{
//			return sizeof(PacketHeader::protocolId) + sizeof(PacketHeader::connectionId) + sizeof(PacketHeader::packetType);
//		}
//
//		void PacketHeader::write(se::BinaryWriter& binaryWriter) const
//		{
//			se_write(binaryWriter, protocolId);
//			se_write(binaryWriter, connectionId);
//			se_write(binaryWriter, packetType);
//			// NOTE: update ReliableFragmentPacket::getPacketPayloadHeaderSize()
//		}
//		bool PacketHeader::read(se::BinaryReader& binaryReader)
//		{
//			se_read(binaryReader, protocolId);
//			se_read(binaryReader, connectionId);
//			se_read(binaryReader, packetType);
//			return true;
//		}
//
//		uint16_t ReliableFragmentPacket::getHeaderSize()
//		{
//			return sizeof(streamOffset) + sizeof(endOfPayload);
//		}
//
//		void ReliableFragmentPacket::writeHeader(se::BinaryWriter& binaryWriter) const
//		{
//			se_write(binaryWriter, streamOffset);
//			se_write(binaryWriter, endOfPayload);
//			// NOTE: remember to update getPacketPayloadHeaderSize()
//		}
//		bool ReliableFragmentPacket::readHeader(se::BinaryReader& binaryReader)
//		{
//			se_read(binaryReader, streamOffset);
//			se_read(binaryReader, endOfPayload);
//			return true;
//		}
//
//		void AcknowledgePacket::write(se::BinaryWriter& binaryWriter) const
//		{
//			se_write(binaryWriter, streamOffset);
//			se_write(binaryWriter, payloadSize);
//		}
//		bool AcknowledgePacket::read(se::BinaryReader& binaryReader)
//		{
//			se_read(binaryReader, streamOffset);
//			se_read(binaryReader, payloadSize);
//			return true;
//		}
//
//		void ConnectPacket::write(se::BinaryWriter& binaryWriter) const
//		{
//			se_write(binaryWriter, connectionId);
//			writeToBuffer<uint8_t>(binaryWriter, debugName);
//		}
//		bool ConnectPacket::read(se::BinaryReader& binaryReader)
//		{
//			se_read(binaryReader, connectionId);
//			readFromBuffer<uint8_t>(binaryReader, debugName);
//			return true;
//		}
//
//		void DisconnectPacket::write(se::BinaryWriter& binaryWriter) const
//		{
//			se_write(binaryWriter, expectingResponse);
//		}
//		bool DisconnectPacket::read(se::BinaryReader& binaryReader)
//		{
//			se_read(binaryReader, expectingResponse);
//			return true;
//		}
//
//		void HeartbeatPacket::write(se::BinaryWriter& binaryWriter) const
//		{
//		}
//		bool HeartbeatPacket::read(se::BinaryReader& binaryReader)
//		{
//			return true;
//		}
//
//		void PathMaximumSegmentSizeDiscoveryPacket::write(se::BinaryWriter& binaryWriter) const
//		{
//			se_write(binaryWriter, size);
//			se_write(binaryWriter, extraPayload);
//		}
//		bool PathMaximumSegmentSizeDiscoveryPacket::read(se::BinaryReader& binaryReader)
//		{
//			se_read(binaryReader, size);
//			se_read(binaryReader, extraPayload);
//			return true;
//		}
//
//		void PathMaximumSegmentSizeAcknowledgePacket::write(se::BinaryWriter& binaryWriter) const
//		{
//			se_write(binaryWriter, size);
//		}
//		bool PathMaximumSegmentSizeAcknowledgePacket::read(se::BinaryReader& binaryReader)
//		{
//			se_read(binaryReader, size);
//			return true;
//		}
//
//		bool UserDataPacket::read(se::BinaryReader& binaryReader)
//		{
//			readFromBuffer<uint8_t, PayloadSizeType>(binaryReader, payload);
//			return true;
//		}
//	}
//}
