// nocommit
//#pragma once
//
//#include "SpehsEngine/Net/ConnectionId.h"
//#include "SpehsEngine/Net/Endpoint.h"
//#include "SpehsEngine/Core/SE_Time.h"
//#include "SpehsEngine/Core/StrongInt.h"
//
//
//namespace se
//{
//	class WriteBuffer;
//	class ReadBuffer;
//
//	namespace net
//	{
//		SE_STRONG_INT(uint32_t, ProtocolId, 0u);
//
//		struct PacketHeader
//		{
//			static const ProtocolId spehsProtocolId;
//
//			enum class PacketType : uint8_t
//			{
//				None,
//				ReliableFragment,
//				Acknowledge,
//				Connect,
//				Disconnect,
//				Heartbeat,
//				PathMaximumSegmentSizeDiscovery,
//				PathMaximumSegmentSizeAcknowledge,
//				UserData,
//			};
//
//			static uint16_t getSize();
//
//			void write(se::BinaryWriter& binaryWriter) const;
//			bool read(se::BinaryReader& binaryReader);
//
//			ProtocolId protocolId;
//			ConnectionId connectionId;
//			PacketType packetType = PacketType::None;
//		};
//
//		struct ReliableFragmentPacket
//		{
//			typedef uint16_t PayloadSizeType;
//			static uint16_t getHeaderSize();
//
//			struct ReadPayload
//			{
//				std::vector<uint8_t> buffer;
//				size_t beginIndex = 0u;
//				PayloadSizeType payloadSize = 0u;
//			};
//
//			// Reliable fragments are sent in scatter & gather buffers where the header and the payload are separated
//			void writeHeader(se::BinaryWriter& binaryWriter) const;
//			bool readHeader(se::BinaryReader& binaryReader);
//
//			ReadPayload readPayload; // Only used when reading, not part of the "header"
//			uint64_t streamOffset = 0u;
//			bool endOfPayload = false;
//		};
//
//		struct AcknowledgePacket
//		{
//			void write(se::BinaryWriter& binaryWriter) const;
//			bool read(se::BinaryReader& binaryReader);
//
//			uint64_t streamOffset = 0u;
//			uint16_t payloadSize = 0u;
//		};
//
//		struct ConnectPacket
//		{
//			void write(se::BinaryWriter& binaryWriter) const;
//			bool read(se::BinaryReader& binaryReader);
//
//			std::string debugName;
//			ConnectionId connectionId;
//		};
//
//		struct DisconnectPacket
//		{
//			void write(se::BinaryWriter& binaryWriter) const;
//			bool read(se::BinaryReader& binaryReader);
//
//			bool expectingResponse = true;
//		};
//
//		struct HeartbeatPacket
//		{
//			void write(se::BinaryWriter& binaryWriter) const;
//			bool read(se::BinaryReader& binaryReader);
//		};
//
//		struct PathMaximumSegmentSizeDiscoveryPacket
//		{
//			void write(se::BinaryWriter& binaryWriter) const;
//			bool read(se::BinaryReader& binaryReader);
//
//			uint16_t size = 0;
//			std::vector<uint8_t> extraPayload;
//		};
//
//		struct PathMaximumSegmentSizeAcknowledgePacket
//		{
//			void write(se::BinaryWriter& binaryWriter) const;
//			bool read(se::BinaryReader& binaryReader);
//
//			uint16_t size = 0;
//		};
//
//		struct UserDataPacket
//		{
//			typedef uint32_t PayloadSizeType;
//			// NOTE: write() is never used for optimization reasons.
//			bool read(se::BinaryReader& binaryReader);
//
//			std::vector<uint8_t> payload; // TODO: optimize? use non-owned memory instead?
//		};
//	}
//}
//
//SE_STRONG_INT_STD(se::net::ProtocolId);
//SE_STRONG_INT_STD(se::net::ConnectionId);