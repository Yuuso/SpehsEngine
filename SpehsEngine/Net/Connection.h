#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/SocketUDP2.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StrongInt.h"
#include "SpehsEngine/Net/ConnectionPackets.h"
#include <functional>
#include <map>
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
				Outgoing,
				Incoming
			};

			enum class Status
			{
				Connecting,
				Connected,
				Disconnected
			};

			struct MutexTimes
			{
				time::Time estimateRoundTripTime;
				time::Time heartbeat;
				time::Time processReceivedRawPackets;
				time::Time deliverOutgoingPackets;
				time::Time deliverReceivedPackets;
				time::Time spehsReceiveHandler;
				time::Time sendPacket;
				time::Time sendPacketImpl;
				time::Time receivePacket;
				time::Time other;
			};

			~Connection();

			void setReceiveHandler(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> callback = std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)>());
			void sendPacket(const WriteBuffer& buffer, const bool reliable);

			void disconnect();
			Status getStatus() const;
			bool isConnected() const;
			boost::asio::ip::udp::endpoint getRemoteEndpoint() const;
			Port getLocalPort() const;

			uint16_t getMaximumSegmentSize() const;
			time::Time getPing() const;
			float getAverageReliableFragmentSendCount() const;
			double getSendQuotaPerSecond() const;
			uint64_t getSentBytes() const;
			uint64_t getSentBytes(const bool reliable) const;
			uint64_t getReceivedBytes() const;
			uint64_t getReceivedBytes(const bool reliable) const;
			std::map<uint64_t, uint64_t> getReliableFragmentSendCounters() const;
			void resetReliableFragmentSendCounters();
			MutexTimes getAcquireMutexTimes() const;
			MutexTimes getHoldMutexTimes() const;
			void resetMutexTimes();

			void connectToStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const Status oldStatus, const Status newStatus)>& callback);

			const std::string& getDebugName() const { return debugName; }
			void setTimeoutEnabled(const bool value);
			bool getTimeoutEnabled() const;
			void setSimulatedPacketLoss(const float chanceToDropIncoming, const float chanceToDropOutgoing, const float chanceToReorderReceivedPacket);
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

			const std::string debugName;
			const std::string debugEndpoint;
			const boost::asio::ip::udp::endpoint endpoint;
			const ConnectionId connectionId;
			const EstablishmentType establishmentType;

		private:

			friend class ConnectionManager;

			struct ReliablePacketOut
			{
				struct UnacknowledgedFragment
				{
					time::Time firstSendTime; // Set when sent the first time. Measurement of ping.
					time::Time latestSendTime; // Set when sent the last time (unacknowledged packets get resent periodically).
					uint64_t sendCount = 0u;
					uint64_t offset = 0u; // Offset to the reliable stream
					uint16_t size = 0u;
				};
				struct AcknowledgedFragment
				{
					uint64_t offset = 0u;
					uint16_t size = 0u;
				};

				ReliablePacketOut() = default;
				ReliablePacketOut(const bool _userData, const uint64_t _payloadOffset, const uint8_t* _payloadPtr, const uint64_t _payloadSize)
					: userData(_userData)
					, payloadOffset(_payloadOffset)
					, payload(size_t(_payloadSize))
				{
					se_assert(_payloadSize > 0);
					se_assert(_payloadSize <= uint64_t(std::numeric_limits<size_t>::max()) && "Payload is too large to fit into std::vector.");
					memcpy(payload.data(), _payloadPtr, size_t(_payloadSize));
				}
				bool userData = false;
				bool delivered = false;
				uint64_t payloadOffset = 0u; // payload's offset in the reliable stream
				std::vector<uint8_t> payload;
				std::vector<UnacknowledgedFragment> unacknowledgedFragments;
				std::vector<AcknowledgedFragment> acknowledgedFragments;
				//Type type = Type::none;
				time::Time createTime = time::now();
			};

			struct UnreliablePacketOut
			{
				UnreliablePacketOut(const uint8_t* _payloadPtr, const uint64_t _payloadSize, const bool _userData)
					: payload(size_t(_payloadSize))
					, userData(_userData)
					, createTime(time::now())
				{
					se_assert(_payloadSize > 0);
					se_assert(_payloadSize <= uint64_t(std::numeric_limits<size_t>::max()) && "Payload is too large to fit into std::vector.");
					memcpy(payload.data(), _payloadPtr, size_t(_payloadSize));
				}
				std::vector<uint8_t> payload;
				bool userData;
				time::Time createTime;
			};

			struct ReceivedReliableFragment
			{
				uint64_t offset = 0u;
				bool userData = false;
				bool endOfPayload = false;
				std::vector<uint8_t> data;
			};
			struct ReceivedReliablePacket
			{
				bool userData = false;
				std::vector<uint8_t> data;
			};

			Connection(const boost::shared_ptr<SocketUDP2>& _socket, const boost::asio::ip::udp::endpoint& _endpoint, const ConnectionId _connectionId,
				const EstablishmentType _establishmentType, const std::string_view _debugName);

			void update(const time::Time timeoutDeltaTime);
			void receiveRawPacket(std::vector<uint8_t>& data);
			void sendPacketImpl(const std::vector<boost::asio::const_buffer>& buffers, const bool logReliable, const bool logUnreliable);
			void sendPacketAcknowledgement(const uint64_t reliableStreamOffset, const uint16_t payloadSize);
			void processReceivedRawPackets();
			void reliableFragmentReceiveHandler(const uint64_t reliableStreamOffset, const uint8_t* const dataPtr, const uint16_t dataSize, const bool userData, const bool endOfPayload, const uint64_t payloadTotalSize);
			void acknowledgementReceiveHandler(const uint64_t reliableStreamOffset, const uint16_t payloadSize);
			void deliverReceivedPackets();
			void deliverOutgoingPackets();
			void spehsReceiveHandler(ReadBuffer& readBuffer);
			void attemptToProcessConnectPacket(ReadBuffer& readBuffer);
			void disconnectImpl(const bool sendDisconnectPacket);
			void setStatus(const Status newStatus);

			/* Declared and defined privately, used by ConnectionManager. */
			bool hasPendingOperations() const;

			void reliableFragmentTransmitted(const uint64_t sendCount, const uint16_t fragmentSize);

			mutable std::recursive_mutex mutex;
			boost::shared_ptr<SocketUDP2> socket;
			std::vector<ReliablePacketOut> reliablePacketSendQueue;
			std::vector<UnreliablePacketOut> unreliablePacketSendQueue;
			std::vector<std::vector<uint8_t>> receivedRawPackets;
			time::Time connectionEstablishedTime;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			time::Time lastSendTimeReliable;
			time::Time lastQueueHeartbeatTime;
			time::Time timeoutCountdown;
			uint64_t reliableStreamOffsetSend = 0u; // bytes from past packets that have been delivered
			uint64_t reliableStreamOffsetReceive = 0u; // bytes from past packets that have been received
			std::vector<ReceivedReliableFragment> receivedReliableFragments;
			std::vector<ReceivedReliablePacket> receivedReliablePackets;
			std::vector<std::vector<uint8_t>> receivedUnreliablePackets;
			std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> receiveHandler;
			Status status = Status::Connecting; // Every connection begins in the connecting state
			ConnectionId remoteConnectionId;
			boost::signals2::signal<void(const Status, const Status)> statusChangedSignal;

			// Congestion avoidance
			struct CongestionAvoidanceState
			{
				// Track how many times a reliable fragment had to be sent in order to go through
				struct ReliableFragmentSendCount
				{
					uint64_t sendCount = 0u;
					uint16_t size = 0u;
				};
				std::vector<ReliableFragmentSendCount> recentReliableFragmentSendCounts;
				std::map<uint64_t, uint64_t> reliableFragmentSendCounters;
				time::Time reEvaluationTimestamp;
				time::Time reEvaluationInterval = time::fromSeconds(1.0f / 10.0f);
				double prevSendQuotaGrowthFactor = 1.0;
				uint64_t prevReliableStreamOffsetSend = 0u;
			};
			CongestionAvoidanceState congestionAvoidanceState;
			double sendQuotaPerSecond = 56600.0; // 56 kbps
			bool moreSendQuotaRequested = false;
			time::Time lastSendQuotaReplenishTimestamp = time::Time::zero;
			uint16_t maximumSegmentSize = 1400u; // 508u;
			uint64_t reliableSendQuota = 0u;
			uint64_t unreliableSendQuota = 0u;
			float averageReliableFragmentSendCount = 0.0f;

			// RTT
			time::Time estimatedRoundTripTime;
			std::vector<time::Time> recentRoundTripTimes;
			bool estimatedRoundTripTimeQueued = false;

			// Time spent locking mutexes
			mutable MutexTimes acquireMutexTimes;
			mutable MutexTimes holdMutexTimes;

			// Transmitted byte counts
			uint64_t sentBytes = 0u;
			uint64_t receivedBytes = 0u;
			uint64_t sentBytesReliable = 0u;
			uint64_t sentBytesUnreliable = 0u;
			uint64_t receivedBytesUnreliable = 0u;

			std::string remoteDebugName;
			int debugLogLevel = 0;
			bool timeoutEnabled = true;
			float simulatedPacketLossChanceIncoming = 0.0f;
			float simulatedPacketLossChanceOutgoing = 0.0f;
			float simulatedPacketChanceReordering = 0.0f;
			std::vector<std::string> debugSelfLog;
		};
	}
}
