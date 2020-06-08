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
			void sendPacket(WriteBuffer& writeBuffer, const bool reliable);

			void setReceiveHandler(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> callback = std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)>());
			
			void disconnect();
			Status getStatus() const;
			bool isConnected() const;
			boost::asio::ip::udp::endpoint getRemoteEndpoint() const;
			Port getLocalPort() const;

			uint16_t getMaximumSegmentSize() const;
			time::Time getPing() const;
			float getAverageReliableFragmentSendCount() const;
			double getSendQuotaPerSecond() const;
			uint64_t getReliableSendQuota() const;
			uint64_t getUnreliableSendQuota() const;
			SentBytes getSentBytes() const;
			ReceivedBytes getReceivedBytes() const;
			uint64_t getReliableUnacknowledgedBytesInQueue() const;
			uint64_t getReliableAcknowledgedBytesInQueue() const;
			uint64_t getReliableStreamOffsetSend() const;
			uint64_t getReliableStreamOffsetReceive() const;
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
					UnacknowledgedFragment(const uint64_t _offset, const uint16_t _size) : offset(_offset), size(_size) {}
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

			struct ReceivedReliableFragment
			{
				/*
					Contains some data along with some parts of the actual payload.
					The payload data can be located using 'payloadIndex' and 'payloadSize'.
					Optimization to avoid reallocating and copying incoming data multiple times.
				*/
				struct PayloadBuffer
				{
					std::vector<uint8_t> buffer;
					size_t payloadIndex = 0u;
					uint16_t payloadSize = 0u;
				};

				ReceivedReliableFragment(const uint64_t _streamOffset, const bool _endOfPayload, std::vector<uint8_t>& _buffer, const size_t _payloadIndex, const uint16_t _payloadSize)
					: streamOffset(_streamOffset)
					, endOfPayload(_endOfPayload)
					, payloadTotalSize(uint64_t(_payloadSize))
				{
					payloadBuffers.push_back(PayloadBuffer());
					payloadBuffers.back().buffer.swap(_buffer);
					payloadBuffers.back().payloadIndex = _payloadIndex;
					payloadBuffers.back().payloadSize = _payloadSize;
				}

				std::vector<PayloadBuffer> payloadBuffers;
				uint64_t streamOffset = 0u;
				bool endOfPayload = false;
				uint64_t payloadTotalSize = 0u; // Tracks the combined size of all payload buffers
			};

			struct ReceivedReliablePacket
			{
				ReceivedReliablePacket(PacketHeader::PacketType _packetType, std::vector<uint8_t>& _payload)
					: packetType(_packetType)
				{
					std::swap(payload, _payload);
				}

				std::vector<uint8_t> payload;
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

			Connection(const boost::shared_ptr<SocketUDP2>& _socket, const boost::asio::ip::udp::endpoint& _endpoint, const ConnectionId _connectionId,
				const EstablishmentType _establishmentType, const std::string_view _debugName);

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
			void disconnectImpl(const bool sendDisconnectPacket);
			void setStatus(const Status newStatus);
			/* Declared and defined privately, used by ConnectionManager. */
			bool hasPendingOperations() const;
			void reliableFragmentTransmitted(const uint64_t sendCount, const uint16_t fragmentSize);
			void increaseSendQuotaPerSecond();
			void decreaseSendQuotaPerSecond();
			void beginPathMaximumSegmentSizeDiscovery();
			void sendNextPathMaximumSegmentSizeDiscoveryPacket();

			mutable std::recursive_mutex mutex;
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
			std::vector<ReceivedReliableFragment> receivedReliableFragments;
			std::vector<ReceivedPacket> receivedPackets;
			std::vector<ReceivedReliablePacket> receivedReliablePackets;
			std::vector<ReceivedPacket> receivedUnreliablePackets;
			std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> receiveHandler;
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
			bool moreSendQuotaRequested = false;
			time::Time lastSendQuotaReplenishTimestamp = time::Time::zero;
			uint16_t maximumSegmentSize = defaultMaximumSegmentSize; // (For outgoing packets only)
			std::optional<PathMaximumSegmentSizeDiscovery> pathMaximumSegmentSizeDiscovery = std::optional<PathMaximumSegmentSizeDiscovery>(PathMaximumSegmentSizeDiscovery());
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
