#pragma once
#include "boost/signals2.hpp"
#include <functional>
#include <mutex>

#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/SocketUDP2.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StrongInt.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		class ConnectionManager;
		class IOService;

		SE_STRONG_INT(uint32_t, ProtocolId, 0u);
		
		class Connection
		{
		public:
			enum class EstablishmentType
			{
				outgoing,
				incoming
			};
		public:

			void setReceiveHandler(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> callback = std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)>());
			void sendPacket(const WriteBuffer& buffer, const bool reliable);

			void stopConnecting();
			void disconnect();
			bool isConnecting() const;
			bool isConnected() const;
			boost::asio::ip::udp::endpoint getRemoteEndpoint() const;
			Port getLocalPort() const;
			time::Time getPing() const;
			size_t getSentBytes() const;
			size_t getSentBytes(const bool reliable) const;
			size_t getReceivedBytes() const;
			size_t getReceivedBytes(const bool reliable) const;

			void connectToConnectionStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const bool)>& callback);

			void setSimulatedPacketLoss(const float chanceToDropIncoming, const float chanceToDropOutgoing);
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

			const std::string debugName;
			const boost::asio::ip::udp::endpoint endpoint;
			const ProtocolId protocolId;
			const EstablishmentType establishmentType;

		private:

			friend class ConnectionManager;

			struct ReliablePacketOut
			{
				enum class Type { none, connect, disconnect, userData };
				WriteBuffer payload;
				Type type = Type::none;
				uint16_t sequenceNumber = 0u;
				size_t sendCount = 0u;
				time::Time latestSendTime; // Set when sent the last time (unacknowledged packets get resent periodically).
				time::Time firstSendTime; // Set when sent the first time. Measurement of ping.
				time::Time createTime = time::now();
			};

			struct UnreliablePacketOut
			{
				WriteBuffer writeBuffer;
			};
			
			Connection(const boost::shared_ptr<SocketUDP2>& _socket, const boost::asio::ip::udp::endpoint& _endpoint,
				const ProtocolId _protocolId, const EstablishmentType _establishmentType, const std::string& _debugName = "Connection");

			void update();
			void receivePacket(std::vector<uint8_t>& data);
			void sendPacketImpl(const std::vector<boost::asio::const_buffer>& buffers, const bool logReliable, const bool logUnreliable);
			void sendPacketAcknowledgement(const uint16_t sequenceNumber);
			void setConnected(const bool value);
			void processIncomingPackets();
			void processOutgoingPackets();

			mutable std::recursive_mutex mutex;
			boost::shared_ptr<SocketUDP2> socket;
			std::vector<ReliablePacketOut> reliablePacketSendQueue;
			std::vector<UnreliablePacketOut> unreliablePacketSendQueue;
			std::vector<std::vector<uint8_t>> receivedPackets;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			size_t mtu = 508u;
			uint16_t sequenceNumber = 0u;
			uint16_t remoteSequenceNumber = 0u;
			std::vector<uint8_t> reliableReceiveBuffer;
			std::vector<std::vector<uint8_t>> receivedReliablePackets;
			std::vector<std::vector<uint8_t>> receivedUnreliablePackets;
			std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> receiveHandler;
			bool connecting = true; // Every connection begins in the connecting state
			bool connected = false;
			boost::signals2::signal<void(const bool)> connectionStatusChangedSignal;

			//Transmitted byte counts
			size_t sentBytes = 0u;
			size_t receivedBytes = 0u;
			size_t sentBytesReliable = 0u;
			size_t sentBytesUnreliable = 0u;
			size_t receivedBytesReliable = 0u;
			size_t receivedBytesUnreliable = 0u;

			//RTT
			time::Time estimatedRoundTripTime;
			std::vector<time::Time> recentRoundTripTimes;
			bool estimatedRoundTripTimeQueued = false;
			
			int debugLogLevel = 0;
			float simulatedPacketLossChanceIncoming = 0.0f;
			float simulatedPacketLossChanceOutgoing = 0.0f;
		};
	}
}
