#pragma once
#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/SocketUDP2.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StrongInt.h"
#include <functional>
#include <mutex>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		class ConnectionManager;
		class IOService;
		
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
			float getAverageReliableFragmentSendCount() const;
			size_t getSentBytes() const;
			size_t getSentBytes(const bool reliable) const;
			size_t getReceivedBytes() const;
			size_t getReceivedBytes(const bool reliable) const;

			void connectToConnectionStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const bool)>& callback);

			void setSimulatedPacketLoss(const float chanceToDropIncoming, const float chanceToDropOutgoing);
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

			const std::string debugName;
			const std::string debugEndpoint;
			const boost::asio::ip::udp::endpoint endpoint;
			const EstablishmentType establishmentType;

		private:

			friend class ConnectionManager;
			
			struct ReliablePacketOut
			{
				struct UnacknowledgedFragment
				{
					time::Time firstSendTime; // Set when sent the first time. Measurement of ping.
					time::Time latestSendTime; // Set when sent the last time (unacknowledged packets get resent periodically).
					size_t sendCount = 0u;
					size_t offset = 0u; // Offset to the reliable stream
					uint16_t size = 0u;
				};
				struct AcknowledgedFragment
				{
					size_t offset = 0u;
					uint16_t size = 0u;
				};
				
				ReliablePacketOut() = default;
				ReliablePacketOut(const bool _userData, const size_t _payloadOffset, const uint8_t* _payloadPtr, const size_t _payloadSize)
					: userData(_userData)
					, payloadOffset(_payloadOffset)
					, payload(_payloadSize)
				{
					se_assert(_payloadSize > 0);
					memcpy(payload.data(), _payloadPtr, _payloadSize);
				}
				bool userData = false;
				bool delivered = false;
				size_t payloadOffset = 0u; // payload's offset in the reliable stream
				std::vector<uint8_t> payload;
				std::vector<UnacknowledgedFragment> unacknowledgedFragments;
				std::vector<AcknowledgedFragment> acknowledgedFragments;
				//Type type = Type::none;
				time::Time createTime = time::now();
			};

			struct UnreliablePacketOut
			{
				UnreliablePacketOut(const uint8_t* _payloadPtr, const size_t _payloadSize, const bool _userData)
					: payload(_payloadSize)
					, userData(_userData)
				{
					se_assert(_payloadSize > 0);
					memcpy(payload.data(), _payloadPtr, _payloadSize);
				}
				std::vector<uint8_t> payload;
				bool userData;
			};

			struct ReceivedFragment
			{
				size_t offset = 0u;
				bool userData = false;
				bool endOfPayload = false;
				std::vector<uint8_t> data;
			};
			
			Connection(const boost::shared_ptr<SocketUDP2>& _socket, const boost::asio::ip::udp::endpoint& _endpoint,
				const EstablishmentType _establishmentType, const std::string& _debugName = "Connection");

			void update();
			void receivePacket(std::vector<uint8_t>& data);
			void sendPacketImpl(const std::vector<boost::asio::const_buffer>& buffers, const bool logReliable, const bool logUnreliable);
			void sendPacketAcknowledgement(const size_t reliableStreamOffset, const uint16_t payloadSize);
			void setConnected(const bool value);
			void processReceivedPackets();
			void reliableFragmentReceiveHandler(const size_t reliableStreamOffset, const uint8_t* const dataPtr, const uint16_t dataSize, const bool userData, const bool endOfPayload, const size_t payloadTotalSize);
			void acknowledgementReceiveHandler(const size_t reliableStreamOffset, const uint16_t payloadSize);
			void deliverReceivedPackets();
			void deliverOutgoingPackets();
			void spehsReceiveHandler(ReadBuffer& readBuffer);

			mutable std::recursive_mutex mutex;
			boost::shared_ptr<SocketUDP2> socket;
			std::vector<ReliablePacketOut> reliablePacketSendQueue;
			std::vector<UnreliablePacketOut> unreliablePacketSendQueue;
			std::vector<std::vector<uint8_t>> receivedPackets;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			size_t mtu = 1400u;// 508u;
			size_t reliableStreamOffsetSend = 0u; // bytes from past packets that have been delivered
			size_t reliableStreamOffsetReceive = 0u; // bytes from past packets that have been received
			std::vector<ReceivedFragment> receivedReliableFragments;
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
			
			//Track how many times a reliable fragment had to be sent in order to go through
			std::vector<std::pair<uint16_t, uint16_t>> recentReliableFragmentSendCounts; // send times + packet size
			float averageReliableFragmentSendCount = 0.0f;

			int debugLogLevel = 0;
			float simulatedPacketLossChanceIncoming = 0.0f;
			float simulatedPacketLossChanceOutgoing = 0.0f;
		};
	}
}
