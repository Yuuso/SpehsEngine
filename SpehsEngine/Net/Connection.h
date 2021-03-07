#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/SocketUDP2.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StrongInt.h"
#include "SpehsEngine/Core/LockGuard.h"
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

		struct ConnectionSimulationSettings
		{
			float chanceToDropIncoming = 0.0f;
			float chanceToDropOutgoing = 0.0f;
			float chanceToReorderReceivedPacket = 0.0f;
			uint16_t maximumSegmentSizeIncoming = std::numeric_limits<uint16_t>::max();
			uint16_t maximumSegmentSizeOutgoing = std::numeric_limits<uint16_t>::max();
		};

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
				Disconnecting,
				Disconnected
			};

			struct MutexTimes
			{
				time::Time estimateRoundTripTime;
				time::Time heartbeat;
				time::Time processReceivedPackets;
				time::Time deliverOutgoingPackets;
				time::Time deliverReceivedPackets;
				time::Time spehsReceiveHandler;
				time::Time sendPacket;
				time::Time sendPacketImpl;
				time::Time receivePacket;
				time::Time debug;
				time::Time other;
			};

			struct SentBytes
			{
				uint64_t raw = 0u;
				uint64_t acknowledgement = 0u;
				uint64_t reliable = 0u; // Does not account reliableResend bytes
				uint64_t reliableResend = 0u;
				uint64_t unreliable = 0u;
				uint64_t pathMaximumSegmentSizeDiscovery = 0u;
			};

			struct ReceivedBytes
			{
				uint64_t raw = 0u; // Does not account overhead from PacketHeader
				uint64_t user = 0u; // Total bytes from user packet contents
			};

			~Connection();

			void sendPacket(const WriteBuffer& writeBuffer, const bool reliable);
			/* Acquires write buffer data and resets given write buffer */
			void sendPacket(WriteBuffer&& acquireWriteBuffer, const bool reliable);

			void setReceiveHandler(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> callback = std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)>());
			
			/* Disconnection cannot be immediately applied so it must be queued. Disconnection happens from the connection manager update. */
			void queueDisconnect();

			void beginPathMaximumSegmentSizeDiscovery();

			Status getStatus() const;
			inline bool isConnecting() const { return getStatus() == Status::Connecting; }
			inline bool isConnected() const { return getStatus() == Status::Connected; }
			inline bool isDisconnecting() const { return getStatus() == Status::Disconnecting; }
			inline bool isDisconnected() const { return getStatus() == Status::Disconnected; }
			boost::asio::ip::udp::endpoint getRemoteEndpoint() const;
			Port getLocalPort() const;

			uint16_t getMaximumSegmentSize() const;
			time::Time getPing() const;
			float getAverageReliableFragmentSendCount() const;
			double getSendQuotaPerSecond() const;
			double getReliableSendQuota() const;
			double getUnreliableSendQuota() const;
			SentBytes getSentBytes() const;
			ReceivedBytes getReceivedBytes() const;
			/* Returns true if there are at least minBytes in the unacknowledged send queue. */
			bool hasReliableUnacknowledgedBytesInQueue(const uint64_t minBytes) const;
			uint64_t getReliableUnacknowledgedBytesInQueue() const;
			uint64_t getReliableSentAcknowledgedBytesInQueue() const;
			uint64_t getReliableReceivedBytesInQueue() const;
			uint64_t getReliableStreamOffsetSend() const;
			uint64_t getReliableStreamOffsetReceive() const;
			uint64_t getSocketSendPacketCallCount() const;
			std::map<uint64_t, uint64_t> getReliableFragmentSendCounters() const;
			void resetReliableFragmentSendCounters();
			MutexTimes getAcquireMutexTimes() const;
			MutexTimes getHoldMutexTimes() const;
			void resetMutexTimes();

			void connectToStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const Status oldStatus, const Status newStatus)>& callback);

			const std::string& getDebugName() const { return debugName; }
			void setTimeoutEnabled(const bool value);
			bool getTimeoutEnabled() const;
			void setConnectionSimulationSettings(const ConnectionSimulationSettings& _simulationSettings);
			ConnectionSimulationSettings getConnectionSimulationSettings() const;
			void setDebugLogLevel(const int level);
			int getDebugLogLevel() const;

			const std::string debugName;
			const std::string debugEndpoint;
			const boost::asio::ip::udp::endpoint endpoint;
			const ConnectionId connectionId;
			const EstablishmentType establishmentType;

		private:

			friend class ConnectionManager;

			static const uint16_t defaultMaximumSegmentSize = 508; // The default, minimum MSS value

			enum class LogSentBytesType
			{
				None, Unreliable, Acknowledgement, Reliable, ReliableResend, PathMaximumSegmentSizeDiscovery
			};

			struct ReliablePacketOut
			{
				struct UnacknowledgedFragment
				{
					UnacknowledgedFragment(const uint64_t _offset, const uint16_t _size) : offset(_offset), size(_size) { se_assert(_size > 0); }
					time::Time firstSendTime; // Set when sent the first time. Measurement of ping.
					time::Time latestSendTime; // Set when sent the last time (unacknowledged packets get resent periodically).
					uint64_t sendCount = 0u;
					uint64_t offset = 0u; // Offset to the reliable stream
					uint16_t size = 0u;
				};

				struct AcknowledgedFragment
				{
					AcknowledgedFragment(const uint64_t _offset, const uint16_t _size) : offset(_offset), size(_size) {}
					uint64_t offset = 0u;
					uint16_t size = 0u;
				};

				ReliablePacketOut() = default;
				ReliablePacketOut(const uint64_t _payloadOffset, WriteBuffer& writeBuffer)
					: payloadOffset(_payloadOffset)
				{
					writeBuffer.swap(payload);
				}

				bool delivered = false;
				uint64_t payloadOffset = 0u; // payload's offset in the reliable stream
				std::vector<uint8_t> payload; // Contains packet type + packet data
				std::vector<UnacknowledgedFragment> unacknowledgedFragments;
				std::vector<AcknowledgedFragment> acknowledgedFragments;
				time::Time createTime = time::now();
			};

			struct UnreliablePacketOut
			{
				UnreliablePacketOut(const PacketHeader::PacketType _packetType, WriteBuffer &writeBuffer)
					: packetType(_packetType)
					, createTime(time::now())
				{
					writeBuffer.swap(payload);
				}

				PacketHeader::PacketType packetType = PacketHeader::PacketType::None;
				std::vector<uint8_t> header;
				std::vector<uint8_t> payload;
				time::Time createTime;
			};

			struct ReceivingReliablePacket
			{
				std::vector<uint8_t> payload;
				uint64_t streamOffset = 0u;
				bool endOfPayload = false;
			};

			struct ReceivedReliablePacket
			{
				ReceivedReliablePacket(PacketHeader::PacketType _packetType, std::vector<uint8_t>& _payloadWithPacketType)
					: packetType(_packetType)
				{
					std::swap(payloadWithPacketType, _payloadWithPacketType);
				}

				std::vector<uint8_t> payloadWithPacketType;
				PacketHeader::PacketType packetType = PacketHeader::PacketType::None;
			};

			struct ReceivedPacket
			{
				ReceivedPacket() = default;
				ReceivedPacket(const PacketHeader& _packetHeader, std::vector<uint8_t>& _buffer, const size_t _payloadOffset)
					: packetHeader(_packetHeader)
					, payloadOffset(_payloadOffset)
				{
					std::swap(buffer, _buffer);
				}

				PacketHeader packetHeader;
				std::vector<uint8_t> buffer;
				size_t payloadOffset = 0u;
			};

			struct PathMaximumSegmentSizeDiscovery
			{
				std::optional<uint16_t> lastSentSize;
				std::optional<uint16_t> maxAcknowledged;
				uint16_t min = defaultMaximumSegmentSize;
				time::Time lastSendTime;
				int iterations = 0;
			};

			struct DisconnectingState
			{
				uint64_t disconnectPacketReliableSendOffset = 0ull;
				bool remoteDisconnectPacketReceived = false;
				bool remoteDisconnectPacketExpected = true;
				time::Time countdown = time::fromSeconds(5.0f);
			};

			Connection(const boost::shared_ptr<SocketUDP2>& _socket, const std::shared_ptr<std::recursive_mutex>& _upperMutex, const boost::asio::ip::udp::endpoint& _endpoint,
				const ConnectionId _connectionId, const ConnectionId _remoteConnectionId, const std::string_view _debugName);

			void update(const time::Time timeoutDeltaTime);
			void receivePacket(const PacketHeader &packetHeader, std::vector<uint8_t>& buffer, const size_t payloadOffset);
			void sendPacketImpl(const std::vector<boost::asio::const_buffer>& buffers, const LogSentBytesType logSentBytesType);
			void logSentBytes(const LogSentBytesType logSentBytesType, const uint64_t bytes);
			bool processReceivedPacket(const PacketHeader::PacketType packetType, std::vector<uint8_t>& buffer, const size_t payloadIndex, const bool reliable);
			void reliableFragmentReceiveHandler(ReliableFragmentPacket& reliableFragmentPacket);
			void sendAcknowledgePacket(const uint64_t reliableStreamOffset, const uint16_t payloadSize);
			void acknowledgementReceiveHandler(const uint64_t reliableStreamOffset, const uint16_t payloadSize);
			void deliverReceivedPackets();
			void deliverReceivedReliablePackets();
			void deliverOutgoingPackets();
			void beginDisconnecting(const std::lock_guard<std::recursive_mutex>& upperLock, const LockGuard<std::recursive_mutex>& lock);
			void updateDisconnecting(const time::Time deltaTime);
			/* lock_guard parameters are here to enforce their locking prior to calling this method. */
			void setStatus(const Status newStatus, const std::lock_guard<std::recursive_mutex> &upperLock, const LockGuard<std::recursive_mutex>& lock);
			/* Declared and defined privately, used by ConnectionManager. */
			bool hasPendingOperations() const;
			void reliableFragmentTransmitted(const uint64_t sendCount, const uint16_t fragmentSize);
			void increaseSendQuotaPerSecond();
			void decreaseSendQuotaPerSecond();
			void sendNextPathMaximumSegmentSizeDiscoveryPacket();

			mutable std::recursive_mutex mutex;
			mutable std::shared_ptr<std::recursive_mutex> upperMutex;
			boost::shared_ptr<SocketUDP2> socket;
			std::vector<ReliablePacketOut> reliablePacketSendQueue;
			std::vector<UnreliablePacketOut> unreliablePacketSendQueue;
			time::Time connectionEstablishedTime;
			time::Time lastReceiveTime;
			time::Time lastSendTime;
			time::Time lastSendTimeReliable;
			time::Time lastQueueHeartbeatTime;
			time::Time timeoutCountdown;
			uint64_t reliableStreamOffsetSend = 0u; // bytes from past packets that have been delivered
			uint64_t reliableStreamOffsetReceive = 0u; // bytes from past packets that have been received
			uint64_t socketSendPacketCallCount = 0u;
			std::vector<ReceivingReliablePacket> receivingReliablePackets;
			std::vector<ReceivedPacket> receivedPackets;
			std::vector<ReceivedReliablePacket> receivedReliablePackets;
			std::vector<ReceivedPacket> receivedUnreliablePackets;
			std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> receiveHandler;
			std::optional<DisconnectingState> disconnectingState;
			Status status = Status::Connecting; // Every connection begins in the connecting state
			ConnectionId remoteConnectionId;
			boost::signals2::signal<void(const Status, const Status)> statusChangedSignal;

			// Track how many times a reliable fragment had to be sent in order to go through
			struct ReliableFragmentSendCount
			{
				uint64_t sendCount = 0u;
				uint16_t size = 0u;
			};
			std::vector<ReliableFragmentSendCount> recentReliableFragmentSendCounts;
			std::map<uint64_t, uint64_t> reliableFragmentSendCounters;
			double sendQuotaPerSecond = 56600.0; // 56 kbps (expected common minimum, according to some shallow research)
			time::Time lastSendQuotaReplenishTimestamp = time::Time::zero;
			uint16_t maximumSegmentSize = defaultMaximumSegmentSize; // (For outgoing packets only)
			std::optional<PathMaximumSegmentSizeDiscovery> pathMaximumSegmentSizeDiscovery = std::optional<PathMaximumSegmentSizeDiscovery>(PathMaximumSegmentSizeDiscovery());
			double reliableSendQuota = 0.0;
			double unreliableSendQuota = 0.0;
			float averageReliableFragmentSendCount = 0.0f;

			// RTT
			time::Time estimatedRoundTripTime;
			std::vector<time::Time> recentRoundTripTimes;
			bool estimatedRoundTripTimeQueued = false;

			// Time spent locking mutexes
			mutable MutexTimes acquireMutexTimes;
			mutable MutexTimes holdMutexTimes;

			// Byte counters
			SentBytes sentBytes;
			ReceivedBytes receivedBytes;

			std::string remoteDebugName;
			int debugLogLevel = 0;
			bool timeoutEnabled = true;
			ConnectionSimulationSettings connectionSimulationSettings;
		};
	}
}
