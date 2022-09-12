#include "stdafx.h"
#include "SpehsEngine/Net/Connection.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/PrecompiledInclude.h"
#include "SpehsEngine/Net/EndpointUtilityFunctions.h"


#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
}

#define LOCK_GUARD(p_name, p_mutex, p_time) \
LockGuard p_name(p_mutex, acquireMutexTimes.p_time, holdMutexTimes.p_time); \
do{} while (false)

//#define LOCK_GUARD(p_name, p_mutex) \
//const se::time::Time p_name##BeginTime = se::time::now(); \
//std::lock_guard<decltype(p_mutex)> p_name(p_mutex); \
//const se::time::Time p_name##EndTime = se::time::now(); \
//const se::time::Time p_name##Duration = p_name##EndTime - p_name##BeginTime; \
//do{} while (false)

//#define LOCK_GUARD(p_name, p_mutex) \
//const se::time::Time p_name##BeginTime = se::time::getProfilerTimestamp(); \
//std::lock_guard<decltype(p_mutex)> p_name(p_mutex); \
//const se::time::Time p_name##EndTime = se::time::getProfilerTimestamp(); \
//const se::time::Time p_name##Duration = p_name##EndTime - p_name##BeginTime; \
//do{} while (false)



namespace se
{
	namespace net
	{
		const time::Time reliableHeartbeatInterval = time::fromSeconds(1.0f);
		const time::Time timeoutTime = reliableHeartbeatInterval * 5;

		Connection::Connection(const boost::shared_ptr<SocketUDP2>& _socket, const std::shared_ptr<std::recursive_mutex>& _upperMutex,
			const boost::asio::ip::udp::endpoint& _endpoint, const ConnectionId _connectionId, ConnectionId _remoteConnectionId,
			const std::string_view _debugName)
			: debugName(_debugName)
			, debugEndpoint(toString(_endpoint))
			, endpoint(_endpoint)
			, connectionId(_connectionId)
			, establishmentType(_remoteConnectionId ? EstablishmentType::Incoming : EstablishmentType::Outgoing)
			, upperMutex(_upperMutex)
			, socket(_socket)
			, remoteConnectionId(_remoteConnectionId)
		{
			LOCK_GUARD(lock, mutex, other);
			ConnectPacket connectPacket;
			connectPacket.connectionId = _connectionId;
			connectPacket.debugName = _debugName;
			WriteBuffer writeBuffer;
			writeBuffer.write(PacketHeader::PacketType::Connect);
			writeBuffer.write(connectPacket);
			reliablePacketSendQueue.push_back(ReliablePacketOut(0u, writeBuffer));

			beginPathMaximumSegmentSizeDiscovery();

#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE // Disable timeouts in dev builds by default
			setTimeoutEnabled(false);
#endif
		}

		Connection::~Connection()
		{
			if (getStatus() != Status::Disconnected)
			{
				// Disconnect packet is sent immediately and unreliably.
				PacketHeader packetHeader;
				packetHeader.protocolId = PacketHeader::spehsProtocolId;
				packetHeader.connectionId = connectionId;
				packetHeader.packetType = PacketHeader::PacketType::Disconnect;

				DisconnectPacket disconnectPacket;
				disconnectPacket.expectingResponse = false;

				WriteBuffer writeBuffer;
				writeBuffer.write(packetHeader);
				writeBuffer.write(disconnectPacket);
				const std::vector<boost::asio::const_buffer> sendBuffers
				{
					boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize())
				};
				sendPacketImpl(sendBuffers, LogSentBytesType::Unreliable);
			}
		}

		void Connection::update(const time::Time timeoutDeltaTime)
		{
			SE_SCOPE_PROFILER(debugName);

			if (!socket->isOpen())
			{
				queueDisconnect();
			}

			if (getStatus() == Status::Disconnecting)
			{
				updateDisconnecting(timeoutDeltaTime);
			}

			deliverReceivedPackets();

			{
				LOCK_GUARD(lock, mutex, estimateRoundTripTime);
				if (estimatedRoundTripTimeQueued)
				{
					estimatedRoundTripTimeQueued = false;
					if (recentRoundTripTimes.empty())
					{
						estimatedRoundTripTime = time::Time::zero;
					}
					else
					{
						float totalMilliseconds = 0.0f;
						for (const time::Time& recentRoundTripTime : recentRoundTripTimes)
						{
							totalMilliseconds += recentRoundTripTime.asMilliseconds();
						}
						estimatedRoundTripTime = time::fromMilliseconds(totalMilliseconds / float(recentRoundTripTimes.size()));
					}
				}

				if (!reliablePacketSendQueue.empty() && !pathMaximumSegmentSizeDiscovery.has_value())
				{
					static const time::Time reliableStreamOffsetSendStuckTime = se::time::fromSeconds(5.0f);
					const time::Time timeSinceUpdate = se::time::now() - lastAdvanceReliableStreamOffsetSendTime;
					if (timeSinceUpdate >= reliableStreamOffsetSendStuckTime)
					{
						DEBUG_LOG(1, "Reliable stream seems to be stuck, initiating path MTU discovery.");
						beginPathMaximumSegmentSizeDiscovery();
					}
				}
			}

			if (getStatus() == Status::Connected)
			{
				bool timeoutTriggered = false;
				{
					// Heartbeat
					LOCK_GUARD(lock, mutex, heartbeat);
					const time::Time now = time::now();
					if (now - lastQueueHeartbeatTime >= reliableHeartbeatInterval &&
						now - lastSendTimeReliable >= reliableHeartbeatInterval)
					{
						lastQueueHeartbeatTime = now;

						HeartbeatPacket heartbeatPacket;
						WriteBuffer writeBuffer;
						writeBuffer.write(PacketHeader::PacketType::Heartbeat);
						writeBuffer.write(heartbeatPacket);

						const uint64_t payloadOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + reliablePacketSendQueue.back().payload.size();
						reliablePacketSendQueue.emplace_back(ReliablePacketOut(payloadOffset, writeBuffer));
					}

					// MSS discovery
					if (pathMaximumSegmentSizeDiscovery)
					{
						const time::Time interval = std::max(time::fromMilliseconds(50.0f), getPing() * 2);
						const time::Time timeSinceLastSend = now - pathMaximumSegmentSizeDiscovery->lastSendTime;
						if (timeSinceLastSend >= interval)
						{
							sendNextPathMaximumSegmentSizeDiscoveryPacket();
						}
					}

					// Timeout disconnection
					timeoutCountdown -= timeoutDeltaTime;
					if (timeoutEnabled && timeoutCountdown <= time::Time::zero)
					{
						timeoutTriggered = true;
					}
				}

				if (timeoutTriggered)
				{
					queueDisconnect();
				}
			}
		}

		void Connection::reliableFragmentReceiveHandler(ReliableFragmentPacket& reliableFragmentPacket)
		{
			const uint16_t payloadSize = reliableFragmentPacket.readPayload.payloadSize;
			LOCK_GUARD(lock, mutex, other);
			sendAcknowledgePacket(reliableFragmentPacket.streamOffset, payloadSize);
			if (reliableFragmentPacket.streamOffset >= reliableStreamOffsetReceive)
			{
				auto postInsert = [&](size_t insertIndex)
				{
					auto checkMergeWithNext = [&](const size_t insertIndex)->bool
					{
						se_assert(insertIndex < receivingReliablePackets.size() - 1);
						// Check if fragment is to be merged with the next fragment
						ReceivingReliablePacket& receivingReliablePacket = receivingReliablePackets[insertIndex];
						if (!receivingReliablePacket.endOfPayload && insertIndex + 1 < receivingReliablePackets.size())
						{
							const uint64_t endOffset = receivingReliablePacket.streamOffset + uint64_t(receivingReliablePacket.payload.size());
							ReceivingReliablePacket& nextReceivingReliablePacket = receivingReliablePackets[insertIndex + 1];
							if (endOffset >= nextReceivingReliablePacket.streamOffset)
							{
								// Absorb the next fragment's contents and erase it
								const uint64_t overlappingSize = endOffset - nextReceivingReliablePacket.streamOffset;

#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
								// Check that overlapping data matches
								if (overlappingSize > 0)
								{
									se_assert(memcmp(receivingReliablePacket.payload.data() + receivingReliablePacket.payload.size() - overlappingSize,
										nextReceivingReliablePacket.payload.data(), overlappingSize) == 0);
								}
#endif

								const size_t oldSize = receivingReliablePacket.payload.size();
								const size_t mergedSize = oldSize + nextReceivingReliablePacket.payload.size() - size_t(overlappingSize);
								const size_t copyBeginIndex = oldSize - overlappingSize;
								se_assert(receivingReliablePacket.payload.size() < mergedSize);
								receivingReliablePacket.payload.resize(mergedSize);
								memcpy(receivingReliablePacket.payload.data() + copyBeginIndex, nextReceivingReliablePacket.payload.data(), nextReceivingReliablePacket.payload.size());
								receivingReliablePacket.endOfPayload = nextReceivingReliablePacket.endOfPayload;

								// Erase "next"
								receivingReliablePackets.erase(receivingReliablePackets.begin() + insertIndex + 1);
								return true;
							}
						}
						return false;
					};

					// Try merging with the previous fragment
					if (insertIndex > 0)
					{
						if (checkMergeWithNext(insertIndex - 1))
						{
							insertIndex--;
						}
					}

					// Try merging with the next fragment
					if (insertIndex + 1 < receivingReliablePackets.size())
					{
						checkMergeWithNext(insertIndex);
					}

					if (reliableFragmentPacket.streamOffset == reliableStreamOffsetReceive && receivingReliablePackets[insertIndex].endOfPayload)
					{
						se_assert(insertIndex == 0 && "reliableStreamOffsetReceive points to the begin, but insertion happened elsewhere?");
					}
				};

				// Receive fragment
				for (size_t f = 0; f < receivingReliablePackets.size(); f++)
				{
					if (reliableFragmentPacket.streamOffset < receivingReliablePackets[f].streamOffset)
					{
						// Insert before
						receivingReliablePackets.insert(receivingReliablePackets.begin() + f, ReceivingReliablePacket());
						receivingReliablePackets[f].streamOffset = reliableFragmentPacket.streamOffset;
						receivingReliablePackets[f].endOfPayload = reliableFragmentPacket.endOfPayload;
						receivingReliablePackets[f].payload.resize(reliableFragmentPacket.readPayload.payloadSize);
						memcpy(receivingReliablePackets[f].payload.data(), reliableFragmentPacket.readPayload.buffer.data() + reliableFragmentPacket.readPayload.beginIndex, reliableFragmentPacket.readPayload.payloadSize);
						postInsert(f);
						return;
					}
					else if (reliableFragmentPacket.streamOffset >= receivingReliablePackets[f].streamOffset && reliableFragmentPacket.streamOffset + payloadSize <= receivingReliablePackets[f].streamOffset + uint64_t(receivingReliablePackets[f].payload.size()))
					{
						// All of this data has been already received
						return;
					}
				}

				receivingReliablePackets.push_back(ReceivingReliablePacket());
				receivingReliablePackets.back().streamOffset = reliableFragmentPacket.streamOffset;
				receivingReliablePackets.back().endOfPayload = reliableFragmentPacket.endOfPayload;
				receivingReliablePackets.back().payload.resize(reliableFragmentPacket.readPayload.payloadSize);
				memcpy(receivingReliablePackets.back().payload.data(), reliableFragmentPacket.readPayload.buffer.data() + reliableFragmentPacket.readPayload.beginIndex, reliableFragmentPacket.readPayload.payloadSize);
				postInsert(receivingReliablePackets.size() - 1);
			}
		}

		void Connection::sendAcknowledgePacket(const uint64_t reliableStreamOffset, const uint16_t payloadSize)
		{
			LOCK_GUARD(lock, mutex, other);
			PacketHeader packetHeader;
			packetHeader.protocolId = PacketHeader::spehsProtocolId;
			packetHeader.connectionId = connectionId;
			packetHeader.packetType = PacketHeader::PacketType::Acknowledge;
			AcknowledgePacket acknowledgePacket;
			acknowledgePacket.streamOffset = reliableStreamOffset;
			acknowledgePacket.payloadSize = payloadSize;
			WriteBuffer writeBuffer;
			writeBuffer.write(packetHeader);
			writeBuffer.write(acknowledgePacket);
			const std::vector<boost::asio::const_buffer> sendBuffers
			{
				boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
			};
			sendPacketImpl(sendBuffers, LogSentBytesType::Acknowledgement);
		}

		void Connection::acknowledgementReceiveHandler(const uint64_t reliableStreamOffset, const uint16_t payloadSize)
		{
			SE_SCOPE_PROFILER(debugName);
			LOCK_GUARD(lock, mutex, other);

			if (reliableStreamOffset >= reliableStreamOffsetSend)
			{
				uint64_t offset = reliableStreamOffsetSend;
				for (size_t p = 0; p < reliablePacketSendQueue.size(); p++)
				{
					se_assert(reliableStreamOffset >= offset);
					if (reliableStreamOffset < offset + reliablePacketSendQueue[p].payload.size())
					{
						// Acknowledgement belongs to this packet, check fragments
						for (size_t f = 0; f < reliablePacketSendQueue[p].unacknowledgedFragments.size(); f++)
						{
							if (reliablePacketSendQueue[p].unacknowledgedFragments[f].offset == reliableStreamOffset &&
								reliablePacketSendQueue[p].unacknowledgedFragments[f].size == payloadSize)
							{
								// Calculate RTT
								recentRoundTripTimes.push_back(time::now() - reliablePacketSendQueue[p].unacknowledgedFragments[f].firstSendTime);
								if (recentRoundTripTimes.size() > 40)
								{
									recentRoundTripTimes.erase(recentRoundTripTimes.begin());
								}
								estimatedRoundTripTimeQueued = true;

								reliableFragmentTransmitted(reliablePacketSendQueue[p].unacknowledgedFragments[f].sendCount, reliablePacketSendQueue[p].unacknowledgedFragments[f].size);

								// Add to acknowledged fragments
								reliablePacketSendQueue[p].acknowledgedFragments.push_back(ReliablePacketOut::AcknowledgedFragment(reliableStreamOffset, payloadSize));

								// Remove from unacknowledged fragments
								if (reliablePacketSendQueue[p].unacknowledgedFragments[f].sendCount > 2 && getDebugLogLevel() >= 2)
								{
									se::log::info("Connection: reliable fragment with " + std::to_string(reliablePacketSendQueue[p].unacknowledgedFragments[f].size) + " bytes was sent " + std::to_string(reliablePacketSendQueue[p].unacknowledgedFragments[f].sendCount) + " times.", se::log::TextColor::YELLOW);
								}
								reliablePacketSendQueue[p].unacknowledgedFragments.erase(reliablePacketSendQueue[p].unacknowledgedFragments.begin() + f);

								// Remove all unacknowledged fragments that have been delivered through other fragments
								// TODO, only happens when MTU changes on the fly

								// Check if the complete payload has been delivered
								if (reliablePacketSendQueue[p].unacknowledgedFragments.empty())
								{
									reliablePacketSendQueue[p].delivered = true;
								}

								break;
							}
						}

						break;
					}
					else if (p + 1 < reliablePacketSendQueue.size())
					{
						// Not in this packet
						offset += reliablePacketSendQueue[p].payload.size();
					}
					else
					{
						se_assert(false && "Acknowledgement refers to a future offset. Should not happen in dev builds but could happen with a hacked connection.");
					}
				}
			}
			else
			{
				// Old acknowledgement, do nothing.
			}
		}

		void Connection::deliverOutgoingPackets()
		{
			SE_SCOPE_PROFILER(debugName);
			const time::Time now = time::now();
			LOCK_GUARD(lock, mutex, deliverOutgoingPackets);

			// Remove delivered reliable packets from the send queue
			{
				SE_SCOPE_PROFILER("remove delivered from send queue");
				while (!reliablePacketSendQueue.empty() && reliablePacketSendQueue.front().delivered)
				{
					lastAdvanceReliableStreamOffsetSendTime = time::now();
					reliableStreamOffsetSend += reliablePacketSendQueue.front().payload.size();
					reliablePacketSendQueue.erase(reliablePacketSendQueue.begin());
				}
			}

			// Replenish send quotas
			if (lastSendQuotaReplenishTimestamp == time::Time::zero)
			{
				lastSendQuotaReplenishTimestamp = now;
			}
			else
			{
				static const time::Time maxTimeSinceReplenish = time::fromSeconds(0.1f);
				const time::Time timeSinceReplenish(std::min((now - lastSendQuotaReplenishTimestamp).value, maxTimeSinceReplenish.value));
				const double secondsSinceReplenish = double(timeSinceReplenish.asSeconds());
				const double newSendQuota = std::ceil(sendQuotaPerSecond * secondsSinceReplenish);
				const double newSendQuotaHalf = newSendQuota * 0.5;
				reliableSendQuota += newSendQuotaHalf;
				unreliableSendQuota += newSendQuotaHalf;
				lastSendQuotaReplenishTimestamp = now;
			}

			// If reliable/unreliable is empty, dump quota to the other. If both are empty, release all so that quota does not stack over time.
			if (reliablePacketSendQueue.empty() && unreliablePacketSendQueue.empty())
			{
				reliableSendQuota = 0.0;
				unreliableSendQuota = 0.0;
			}
			else if (reliablePacketSendQueue.empty())
			{
				unreliableSendQuota += reliableSendQuota;
				reliableSendQuota = 0.0;
			}
			else if (unreliablePacketSendQueue.empty())
			{
				reliableSendQuota += unreliableSendQuota;
				unreliableSendQuota = 0.0;
			}

			reliableSendQuota = std::min(reliableSendQuota, double(maximumSegmentSize * 10));
			unreliableSendQuota = std::min(unreliableSendQuota, double(maximumSegmentSize * 10));

			// Reliable send queue
			{
				SE_SCOPE_PROFILER("reliable send queue");
				static const uint16_t totalHeaderSize = PacketHeader::getSize() + ReliableFragmentPacket::getHeaderSize() + sizeof(ReliableFragmentPacket::PayloadSizeType);
				for (ReliablePacketOut& reliablePacketOut : reliablePacketSendQueue)
				{
					// Generate unacknowledged fragments
					if (!reliablePacketOut.delivered && reliablePacketOut.unacknowledgedFragments.empty())
					{
						SE_SCOPE_PROFILER("generate unacknowledged fragments");
						se_assert(maximumSegmentSize > totalHeaderSize);
						const uint64_t packetOffsetEnd = reliablePacketOut.payloadOffset + reliablePacketOut.payload.size();
						const uint16_t maxSendBufferSize = 60000;
						const uint64_t maxFragmentSize = uint64_t(maximumSegmentSize - totalHeaderSize);
						const uint64_t maxFragmentPayloadSize = std::min(uint64_t(maxSendBufferSize - totalHeaderSize), maxFragmentSize);
						if (reliablePacketOut.acknowledgedFragments.empty())
						{
							const uint64_t totalPayloadSize = uint64_t(reliablePacketOut.payload.size());
							const size_t estimatedFragmentCount = totalPayloadSize / maxFragmentPayloadSize + 1;
							reliablePacketOut.unacknowledgedFragments.reserve(estimatedFragmentCount);
							uint64_t remaining = totalPayloadSize;
							while (remaining > 0)
							{
								const uint16_t fragmentSize = uint16_t(std::min(remaining, maxFragmentPayloadSize));
								reliablePacketOut.unacknowledgedFragments.push_back(ReliablePacketOut::UnacknowledgedFragment(packetOffsetEnd - remaining, fragmentSize));
								remaining -= fragmentSize;
							}
						}
						else
						{
							// Some parts of the packet are already transmitted, generate fragments only for missing sections of the payload
							uint64_t fragmentOffsetBegin = reliablePacketOut.payloadOffset;
							while (true)
							{
								uint64_t remainingUnchecked = packetOffsetEnd - fragmentOffsetBegin;
								uint64_t fragmentOffsetEnd = fragmentOffsetBegin + std::min(remainingUnchecked, maxFragmentSize);
								se_assert(fragmentOffsetBegin <= fragmentOffsetEnd);
								for (size_t f = 0; f < reliablePacketOut.acknowledgedFragments.size();)
								{
									const ReliablePacketOut::AcknowledgedFragment& acknowledgedFragment = reliablePacketOut.acknowledgedFragments[f];
									const uint64_t acknowledgedFragmentOffsetEnd = acknowledgedFragment.offset + uint64_t(acknowledgedFragment.size);
									if (fragmentOffsetBegin >= acknowledgedFragment.offset && fragmentOffsetBegin < acknowledgedFragmentOffsetEnd)
									{
										fragmentOffsetBegin = acknowledgedFragmentOffsetEnd;
										remainingUnchecked = packetOffsetEnd - fragmentOffsetBegin;
										fragmentOffsetEnd = fragmentOffsetBegin + std::min(remainingUnchecked, maxFragmentSize);
										se_assert(fragmentOffsetBegin <= fragmentOffsetEnd);
										f = 0; // Restart the search because fragments aren't in order and there may be an earlier fragment covering the new payload area
									}
									else
									{
										if (fragmentOffsetBegin < acknowledgedFragment.offset && fragmentOffsetEnd > acknowledgedFragment.offset)
										{
											fragmentOffsetEnd = acknowledgedFragment.offset;
											se_assert(fragmentOffsetBegin <= fragmentOffsetEnd);
										}
										f++;
									}
								}
								if (fragmentOffsetEnd > fragmentOffsetBegin)
								{
									const uint16_t fragmentSize = uint16_t(fragmentOffsetEnd - fragmentOffsetBegin);
									se_assert(fragmentSize <= maxFragmentSize);
									reliablePacketOut.unacknowledgedFragments.push_back(ReliablePacketOut::UnacknowledgedFragment(fragmentOffsetBegin, fragmentSize));
									fragmentOffsetBegin = fragmentOffsetEnd;
								}
								else
								{
									se_assert(fragmentOffsetEnd == fragmentOffsetBegin);
									break;
								}
							}

#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
							// Check coherency
							uint64_t offsetBegin = reliablePacketOut.payloadOffset;
							while (offsetBegin < reliablePacketOut.payloadOffset + reliablePacketOut.payload.size())
							{
								const size_t prevOffsetBegin = offsetBegin;
								for (const ReliablePacketOut::AcknowledgedFragment& acknowledgedFragment : reliablePacketOut.acknowledgedFragments)
								{
									if (acknowledgedFragment.offset <= offsetBegin && offsetBegin < acknowledgedFragment.offset + acknowledgedFragment.size)
									{
										offsetBegin = acknowledgedFragment.offset + acknowledgedFragment.size;
									}
								}
								for (const ReliablePacketOut::UnacknowledgedFragment& unacknowledgedFragment : reliablePacketOut.unacknowledgedFragments)
								{
									if (unacknowledgedFragment.offset <= offsetBegin && offsetBegin < unacknowledgedFragment.offset + unacknowledgedFragment.size)
									{
										offsetBegin = unacknowledgedFragment.offset + unacknowledgedFragment.size;
									}
								}
								se_assert(prevOffsetBegin != offsetBegin);
							}
							se_assert(offsetBegin == reliablePacketOut.payloadOffset + reliablePacketOut.payload.size());
#endif
						}
					}

					// Keep (re)sending unacknowledged fragments
					bool awaitForMoreSendQuota = false;
					const time::Time capResendTime = se::time::fromSeconds(0.2f); // Do not wait longer than this to resend
					const time::Time resendTime = estimatedRoundTripTime != time::Time::zero
						? std::min(time::Time(estimatedRoundTripTime.value + estimatedRoundTripTime.value / 6), capResendTime)
						: capResendTime;
					//const time::Time resendTime = estimatedRoundTripTime != time::Time::zero ? estimatedRoundTripTime.value + estimatedRoundTripTime.value / 6 : capResendTime;
					for (size_t f = 0; f < reliablePacketOut.unacknowledgedFragments.size(); f++)
					{
						ReliablePacketOut::UnacknowledgedFragment& unacknowledgedFragment = reliablePacketOut.unacknowledgedFragments[f];
						const se::time::Time timeSinceSend = now - unacknowledgedFragment.latestSendTime;
						if (timeSinceSend >= resendTime)
						{
							if (totalHeaderSize + unacknowledgedFragment.size <= maximumSegmentSize)
							{
								// Packet is valid for (re)send
								PacketHeader packetHeader;
								packetHeader.protocolId = PacketHeader::spehsProtocolId;
								packetHeader.connectionId = connectionId;
								packetHeader.packetType = PacketHeader::PacketType::ReliableFragment;
								ReliableFragmentPacket reliableFragmentPacket;
								reliableFragmentPacket.streamOffset = unacknowledgedFragment.offset;
								reliableFragmentPacket.endOfPayload = (unacknowledgedFragment.offset + unacknowledgedFragment.size) == (reliablePacketOut.payloadOffset + reliablePacketOut.payload.size());

								// Optimized write: do not write payload, use scatter & gather buffers
								WriteBuffer writeBuffer;
								writeBuffer.write(packetHeader);
								reliableFragmentPacket.writeHeader(writeBuffer);
								ReliableFragmentPacket::PayloadSizeType payloadSize = unacknowledgedFragment.size;
								writeBuffer.write(payloadSize);

								const size_t internalOffset = size_t(unacknowledgedFragment.offset - reliablePacketOut.payloadOffset);
								const std::vector<boost::asio::const_buffer> sendBuffers
								{
									boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
									boost::asio::const_buffer(reliablePacketOut.payload.data() + internalOffset, size_t(unacknowledgedFragment.size))
								};
								const size_t sendBuffersSize = boost::asio::buffer_size(sendBuffers);

								if (double(sendBuffersSize) < reliableSendQuota)
								{
									sendPacketImpl(sendBuffers, unacknowledgedFragment.sendCount > 0 ? LogSentBytesType::ReliableResend : LogSentBytesType::Reliable);
									reliableSendQuota -= double(sendBuffersSize);

									if (unacknowledgedFragment.sendCount++ == 0)
									{
										unacknowledgedFragment.firstSendTime = now;
									}
									else
									{
										decreaseSendQuotaPerSecond();
									}

									unacknowledgedFragment.latestSendTime = now;
								}
								else
								{
									// Not enough send quota. Break, we want to buffer up send quota to send this fragment asap.
									awaitForMoreSendQuota = true;
									break;
								}
							}
							else
							{
								// Packet is too big to fit the (changed?) mss -> split
								const uint64_t originalOffset = unacknowledgedFragment.offset;
								const uint16_t size1 = unacknowledgedFragment.size / 2;
								const uint16_t size2 = unacknowledgedFragment.size - size1;
								reliablePacketOut.unacknowledgedFragments.insert(reliablePacketOut.unacknowledgedFragments.begin() + f + 1,
									ReliablePacketOut::UnacknowledgedFragment(originalOffset + size1, size2));
								unacknowledgedFragment = ReliablePacketOut::UnacknowledgedFragment(originalOffset, size1);
								f--; // Redo this iteration
							}
						}
					}

					if (awaitForMoreSendQuota)
					{
						break;
					}
				}
			}

			// Unreliable send queue
			{
				SE_SCOPE_PROFILER("unreliable send queue");
				while (!unreliablePacketSendQueue.empty())
				{
					PacketHeader packetHeader;
					packetHeader.protocolId = PacketHeader::spehsProtocolId;
					packetHeader.connectionId = connectionId;
					packetHeader.packetType = unreliablePacketSendQueue.front().packetType;
					WriteBuffer writeBuffer;
					writeBuffer.write(packetHeader);
					if (writeBuffer.getSize() + unreliablePacketSendQueue.front().payload.size() <= uint64_t(unreliableSendQuota))
					{
						const std::vector<boost::asio::const_buffer> sendBuffers
						{
							boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
							boost::asio::const_buffer(unreliablePacketSendQueue.front().header.data(), unreliablePacketSendQueue.front().header.size()),
							boost::asio::const_buffer(unreliablePacketSendQueue.front().payload.data(), unreliablePacketSendQueue.front().payload.size()),
						};
						sendPacketImpl(sendBuffers, LogSentBytesType::Unreliable);
						unreliableSendQuota -= double(boost::asio::buffer_size(sendBuffers));
						unreliablePacketSendQueue.erase(unreliablePacketSendQueue.begin());
					}
					else
					{
						break;
					}
				}
			}

			// Drop all unreliable packets that are getting stale
			{
				SE_SCOPE_PROFILER("drop stale unreliable packets");
				static const time::Time staleTimeLimit = se::time::fromSeconds(0.1f);
				for (size_t i = 0; i < unreliablePacketSendQueue.size();)
				{
					if (now - unreliablePacketSendQueue[i].createTime > staleTimeLimit)
					{
						unreliablePacketSendQueue.erase(unreliablePacketSendQueue.begin() + i);
					}
					else
					{
						i++;
					}
				}
			}
		}

		void Connection::deliverReceivedPackets()
		{
			SE_SCOPE_PROFILER(debugName);

			// Simulated packet reordering
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
			{
				LOCK_GUARD(lock, mutex, debug);
				if (connectionSimulationSettings.chanceToReorderReceivedPacket > 0.0f)
				{
					SE_SCOPE_PROFILER("simulated reordering");
					if (receivedPackets.size() < 2)
					{
						// We do not process received packets until 2+ have arrived so that we can give the arrived packet a chance to be reordered
						return;
					}
					for (size_t i = 1; i < receivedPackets.size(); i++)
					{
						if (rng::weightedCoin(connectionSimulationSettings.chanceToReorderReceivedPacket))
						{
							// Swap with the previous packet
							std::swap(receivedPackets[i - 1], receivedPackets[i]);
						}
					}
				}
			}
#endif

			// Lock mutex and process all currently received packets
			{
				SE_SCOPE_PROFILER("process received packets");
				std::lock_guard<std::recursive_mutex> upperLock(*upperMutex);
				LOCK_GUARD(lock, mutex, processReceivedPackets);
				while (receivedPackets.size() > 0)
				{
					ReceivedPacket receivedPacket;
					std::swap(receivedPacket, receivedPackets.front());
					receivedPackets.erase(receivedPackets.begin());
					processReceivedPacket(receivedPacket.packetHeader.packetType, receivedPacket.buffer, receivedPacket.payloadOffset, false);
				}
			}

			// Gather received reliable packets
			{
				SE_SCOPE_PROFILER("gather received packets");
				LOCK_GUARD(lock, mutex, deliverReceivedPackets);
				while (!receivingReliablePackets.empty() && receivingReliablePackets.front().endOfPayload && receivingReliablePackets.front().streamOffset == reliableStreamOffsetReceive)
				{
					se_assert(receivingReliablePackets.front().payload.size() > 0);
					reliableStreamOffsetReceive += receivingReliablePackets.front().payload.size();
					ReadBuffer readBuffer(receivingReliablePackets.front().payload.data(), receivingReliablePackets.front().payload.size());
					PacketHeader::PacketType packetType = PacketHeader::PacketType::None;
					if (readBuffer.read(packetType))
					{
						receivedReliablePackets.emplace_back(ReceivedReliablePacket(packetType, receivingReliablePackets.front().payload));
					}
					else
					{
						se::log::error("Could not rerad the received reliable packet's packet type. Packet cannot be delivered.");
					}
					receivingReliablePackets.erase(receivingReliablePackets.begin());
				}
			}

			deliverReceivedReliablePackets();
		}

		void Connection::deliverReceivedReliablePackets()
		{
			SE_SCOPE_PROFILER();
			std::lock_guard<std::recursive_mutex> upperLock(*upperMutex);
			LOCK_GUARD(lock, mutex, deliverReceivedPackets);
			while (true)
			{
				if (receivedReliablePackets.empty())
				{
					return;
				}
				else
				{
					const size_t packetTypeSize = sizeof(PacketHeader::PacketType);
					if (processReceivedPacket(receivedReliablePackets.front().packetType, receivedReliablePackets.front().payloadWithPacketType, packetTypeSize, true))
					{
						receivedReliablePackets.erase(receivedReliablePackets.begin());
					}
					else
					{
						// Must wait until a receive handler is specified
						if (!receiveHandler && receivedReliablePackets.size() > 1000)
						{
							log::warning(debugName + std::to_string(receivedReliablePackets.size()) + " received reliable packets awaiting to be deliver. Use se::net::Connection::setReceiveHandler() to set the receive handler.");
						}
						return;
					}
				}
			}
		}

		bool Connection::processReceivedPacket(const PacketHeader::PacketType packetType, std::vector<uint8_t>& buffer, const size_t payloadIndex, const bool reliable)
		{
			std::lock_guard<std::recursive_mutex> upperLock(*upperMutex);
			LOCK_GUARD(lock, mutex, processReceivedPackets);

			ReadBuffer readBuffer(buffer.data() + payloadIndex, buffer.size() - payloadIndex);

			switch (packetType)
			{
			case PacketHeader::PacketType::None:
				return false;

			case PacketHeader::PacketType::UserData:
			{
				UserDataPacket userDataPacket;
				if (readBuffer.read(userDataPacket))
				{
					receivedBytes.user += readBuffer.getSize();
					if (receiveHandler)
					{
						SE_SCOPE_PROFILER("user data handler");
						ReadBuffer userReadBuffer(userDataPacket.payload.data(), userDataPacket.payload.size());
						receiveHandler(userReadBuffer, endpoint, reliable);
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					se::log::warning("Failed to read user data packet.");
					return false;
				}
			}

			case PacketHeader::PacketType::ReliableFragment:
			{
				ReliableFragmentPacket reliableFragmentPacket;
				if (reliableFragmentPacket.readHeader(readBuffer))
				{
					if (readBuffer.read(reliableFragmentPacket.readPayload.payloadSize))
					{
						reliableFragmentPacket.readPayload.beginIndex = payloadIndex + readBuffer.getOffset();
						reliableFragmentPacket.readPayload.buffer.swap(buffer);
						reliableFragmentReceiveHandler(reliableFragmentPacket);
						return true;
					}
					else
					{
						se::log::warning("Failed to read reliable fragment payload.");
						return false;
					}
				}
				else
				{
					se::log::warning("Failed to read reliable fragment header.");
					return false;
				}
			}

			case PacketHeader::PacketType::Acknowledge:
			{
				AcknowledgePacket acknowledgePacket;
				if (readBuffer.read(acknowledgePacket))
				{
					acknowledgementReceiveHandler(acknowledgePacket.streamOffset, acknowledgePacket.payloadSize);
					return true;
				}
				else
				{
					se::log::warning("Failed to read acknowledge packet.");
					return false;
				}
			}

			case PacketHeader::PacketType::Heartbeat:
			{
				HeartbeatPacket heartbeatPacket;
				if (readBuffer.read(heartbeatPacket))
				{
					// Do nothing
					return true;
				}
				else
				{
					se::log::warning("Failed to read heartbeat packet.");
					return false;
				}
			}

			case PacketHeader::PacketType::Disconnect:
			{
				DisconnectPacket disconnectPacket;
				if (readBuffer.read(disconnectPacket))
				{
					beginDisconnecting(upperLock, lock);
					if (disconnectingState)
					{
						disconnectingState->remoteDisconnectPacketReceived = true;
						disconnectingState->remoteDisconnectPacketExpected = disconnectPacket.expectingResponse;
					}
					return true;
				}
				else
				{
					se::log::warning("Failed to read disconnect packet.");
					return false;
				}
			}

			case PacketHeader::PacketType::Connect:
			{
				ConnectPacket connectPacket;
				if (readBuffer.read(connectPacket))
				{
					if (connectPacket.connectionId)
					{
						switch (establishmentType)
						{
						case EstablishmentType::Incoming:
							se_assert(remoteConnectionId == connectPacket.connectionId);
							break;
						case EstablishmentType::Outgoing:
							se_assert(!remoteConnectionId);
							remoteConnectionId = connectPacket.connectionId;
						}
						remoteDebugName = connectPacket.debugName;
						DEBUG_LOG(1, "connected.");
						setStatus(Status::Connected, upperLock, lock);
					}
					else
					{
						log::warning("ConnectPacket packet does not define connection id.");
					}
					return true;
				}
				else
				{
					se::log::warning("Failed to read connect packet.");
					return false;
				}
			}

			case PacketHeader::PacketType::PathMaximumSegmentSizeDiscovery:
			{
				PathMaximumSegmentSizeDiscoveryPacket pathMaximumSegmentSizeDiscoveryPacket;
				if (readBuffer.read(pathMaximumSegmentSizeDiscoveryPacket))
				{
					PacketHeader packetHeader;
					packetHeader.protocolId = PacketHeader::spehsProtocolId;
					packetHeader.connectionId = connectionId;
					packetHeader.packetType = PacketHeader::PacketType::PathMaximumSegmentSizeAcknowledge;
					WriteBuffer writeBuffer;
					writeBuffer.write(packetHeader);

					PathMaximumSegmentSizeAcknowledgePacket pathMaximumSegmentSizeAcknowledgePacket;
					pathMaximumSegmentSizeAcknowledgePacket.size = pathMaximumSegmentSizeDiscoveryPacket.size;
					writeBuffer.write(pathMaximumSegmentSizeAcknowledgePacket);

					const std::vector<boost::asio::const_buffer> sendBuffers
					{
						boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
					};
					sendPacketImpl(sendBuffers, LogSentBytesType::PathMaximumSegmentSizeDiscovery);
					return true;
				}
				else
				{
					se::log::warning("Failed to read path maximum segment size discovery packet.");
					return false;
				}
			}

			case PacketHeader::PacketType::PathMaximumSegmentSizeAcknowledge:
			{
				PathMaximumSegmentSizeAcknowledgePacket pathMaximumSegmentSizeAcknowledgePacket;
				if (readBuffer.read(pathMaximumSegmentSizeAcknowledgePacket))
				{
					if (pathMaximumSegmentSizeAcknowledgePacket.size > maximumSegmentSize)
					{
						if (connectionSimulationSettings.maximumSegmentSizeOutgoing == 0u || pathMaximumSegmentSizeAcknowledgePacket.size <= connectionSimulationSettings.maximumSegmentSizeOutgoing)
						{
							maximumSegmentSize = pathMaximumSegmentSizeAcknowledgePacket.size;
							DEBUG_LOG(2, "Maximum segment size updated: " + std::to_string(pathMaximumSegmentSizeAcknowledgePacket.size));

							if (pathMaximumSegmentSizeDiscovery)
							{
								if (pathMaximumSegmentSizeDiscovery->maxAcknowledged)
								{
									pathMaximumSegmentSizeDiscovery->maxAcknowledged = std::max(*pathMaximumSegmentSizeDiscovery->maxAcknowledged, pathMaximumSegmentSizeAcknowledgePacket.size);
								}
								else
								{
									pathMaximumSegmentSizeDiscovery->maxAcknowledged = pathMaximumSegmentSizeAcknowledgePacket.size;
								}
							}
						}
					}
					return true;
				}
				else
				{
					se::log::warning("Failed to read path maximum segment size discovery packet.");
					return false;
				}
			}
			}

			se::log::warning("Connection::processReceivedPacket() Unknown PacketType: " + std::to_string(int(packetType)));
			return false;
		}

		void Connection::setReceiveHandler(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> callback)
		{
			LOCK_GUARD(lock, mutex, other);
			receiveHandler = callback;
		}

		void Connection::sendPacket(const WriteBuffer& writeBuffer, const bool reliable)
		{
			WriteBuffer writeBufferCopy = writeBuffer;
			sendPacket(std::move(writeBufferCopy), reliable);
		}

		void Connection::sendPacket(WriteBuffer&& writeBuffer, const bool reliable)
		{
			// NOTE: this function is used for user packets only
			SE_SCOPE_PROFILER(debugName);
			if (writeBuffer.isEmpty())
			{
				return;
			}
			LOCK_GUARD(lock, mutex, sendPacket);
			if (status == Status::Connected)
			{
				if (reliable)
				{
					// Prepare buffer prefix contents
					const UserDataPacket::PayloadSizeType payloadSize = UserDataPacket::PayloadSizeType(writeBuffer.getSize());
					WriteBuffer writeBufferPrefix;
					writeBufferPrefix.write(PacketHeader::PacketType::UserData);
					writeBufferPrefix.write(payloadSize);
					std::vector<uint8_t> writeBufferPrefixBuffer;
					writeBufferPrefix.swap(writeBufferPrefixBuffer);

					// Optimized prefixing of user packet payload with the packet type and payload size. We can most likely avoid dynamic allocation but we will have to shift the entire buffer forward...
					std::vector<uint8_t> buffer;
					writeBuffer.swap(buffer);
					writeBufferPrefix.write(PacketHeader::PacketType::UserData);
					writeBufferPrefix.write(PacketHeader::PacketType::UserData);
					buffer.reserve(writeBufferPrefixBuffer.size() + buffer.size());
					buffer.insert(buffer.begin(), writeBufferPrefixBuffer.begin(), writeBufferPrefixBuffer.end());
					writeBuffer.swap(buffer);

					const uint64_t payloadOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + uint64_t(reliablePacketSendQueue.back().payload.size());
					reliablePacketSendQueue.push_back(ReliablePacketOut(payloadOffset, writeBuffer));
				}
				else
				{
					const UserDataPacket::PayloadSizeType userDataPayloadSize = UserDataPacket::PayloadSizeType(writeBuffer.getSize());
					unreliablePacketSendQueue.push_back(UnreliablePacketOut(PacketHeader::PacketType::UserData, writeBuffer));
					WriteBuffer userDataHeaderWriteBuffer;
					userDataHeaderWriteBuffer.write(userDataPayloadSize);
					userDataHeaderWriteBuffer.swap(unreliablePacketSendQueue.back().header);
				}
			}
			else
			{
				se_assert(false && "Connection is not in the connected state. Cannot send packet.");
				se::log::warning("Connection is not in the connected state. Cannot send packet.");
			}
		}

		void Connection::sendPacketImpl(const std::vector<boost::asio::const_buffer>& buffers, const LogSentBytesType logSentBytesType)
		{
			const uint64_t bufferSize = uint64_t(boost::asio::buffer_size(buffers));

			LOCK_GUARD(lock, mutex, sendPacketImpl);
			se_assert(bufferSize <= maximumSegmentSize || (logSentBytesType != LogSentBytesType::ReliableResend && logSentBytesType != LogSentBytesType::Reliable) && "Reliable packets should be split properly by now according to the current MSS.");
			lastSendTime = time::now();
			if (logSentBytesType == LogSentBytesType::Reliable || logSentBytesType == LogSentBytesType::ReliableResend)
			{
				lastSendTimeReliable = lastSendTime;
			}
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
			if ((connectionSimulationSettings.chanceToDropOutgoing > 0.0f && rng::weightedCoin(connectionSimulationSettings.chanceToDropOutgoing))
				|| (connectionSimulationSettings.maximumSegmentSizeOutgoing > 0 && bufferSize > connectionSimulationSettings.maximumSegmentSizeOutgoing))
			{
				logSentBytes(logSentBytesType, bufferSize);
				return;
			}
#endif
			socket->sendPacket(buffers, endpoint);
			socketSendPacketCallCount++;
			logSentBytes(logSentBytesType, bufferSize);
		}

		void Connection::logSentBytes(const LogSentBytesType logSentBytesType, const uint64_t bytes)
		{
			sentBytes.raw += bytes;
			switch (logSentBytesType)
			{
			case LogSentBytesType::None:
				break;
			case LogSentBytesType::Acknowledgement:
				sentBytes.acknowledgement += bytes;
				break;
			case LogSentBytesType::Unreliable:
				sentBytes.unreliable += bytes;
				break;
			case LogSentBytesType::Reliable:
				sentBytes.reliable += bytes;
				break;
			case LogSentBytesType::ReliableResend:
				sentBytes.reliableResend += bytes;
				break;
			case LogSentBytesType::PathMaximumSegmentSizeDiscovery:
				sentBytes.pathMaximumSegmentSizeDiscovery += bytes;
				break;
			}
		}

		void Connection::queueDisconnect()
		{
			std::lock_guard<std::recursive_mutex> upperLock(*upperMutex);
			LOCK_GUARD(lock, mutex, other);
			if (!disconnectingState && status != Status::Disconnected)
			{
				beginDisconnecting(upperLock, lock);
			}
		}

		void Connection::beginDisconnecting(const std::lock_guard<std::recursive_mutex>& upperLock, const LockGuard<std::recursive_mutex>& lock)
		{
			if (status != Status::Disconnected && status != Status::Disconnecting)
			{
				disconnectingState.emplace();
				setStatus(Status::Disconnecting, upperLock, lock);
				
				// Place the disconnection packet onto the reliable send queue
				const uint64_t reliableStreamOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + uint64_t(reliablePacketSendQueue.back().payload.size());
				DisconnectPacket disconnectPacket;
				WriteBuffer writeBuffer;
				writeBuffer.write(PacketHeader::PacketType::Disconnect);
				writeBuffer.write(disconnectPacket);
				reliablePacketSendQueue.push_back(ReliablePacketOut(reliableStreamOffset, writeBuffer));

				disconnectingState->disconnectPacketReliableSendOffset = reliableStreamOffset;
			}
		}

		void Connection::updateDisconnecting(const time::Time deltaTime)
		{
			std::lock_guard<std::recursive_mutex> upperLock(*upperMutex);
			LOCK_GUARD(lock, mutex, other);
			if (status == Status::Disconnecting)
			{
				se_assert(disconnectingState);
				const bool disconnectPacketDeliveryFinished = reliableStreamOffsetSend > disconnectingState->disconnectPacketReliableSendOffset
					|| !disconnectingState->remoteDisconnectPacketExpected;
				if (disconnectPacketDeliveryFinished && disconnectingState->remoteDisconnectPacketReceived)
				{
					disconnectingState->countdown -= deltaTime;
					if (disconnectingState->countdown <= time::Time::zero)
					{
						DEBUG_LOG(1, "disconnected.");
						disconnectingState.reset();
						setStatus(Status::Disconnected, upperLock, lock);
					}
				}
			}
		}

		void Connection::receivePacket(const PacketHeader& packetHeader, std::vector<uint8_t>& buffer, const size_t payloadOffset)
		{
			LOCK_GUARD(lock, mutex, receivePacket);
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
			if (connectionSimulationSettings.chanceToDropIncoming > 0.0f && rng::weightedCoin(connectionSimulationSettings.chanceToDropIncoming))
			{
				return;
			}
			if (connectionSimulationSettings.maximumSegmentSizeOutgoing > 0 && buffer.size() > connectionSimulationSettings.maximumSegmentSizeIncoming)
			{
				return;
			}
#endif
			lastReceiveTime = time::now();
			timeoutCountdown = timeoutTime;
			receivedBytes.raw += buffer.size();
			receivedPackets.push_back(ReceivedPacket(packetHeader, buffer, payloadOffset));
		}

		void Connection::reliableFragmentTransmitted(const uint64_t sendCount, const uint16_t fragmentSize)
		{
			LOCK_GUARD(lock, mutex, other);

			if (sendCount == 1)
			{
				increaseSendQuotaPerSecond();
			}

			// Add to recentReliableFragmentSendCounts
			recentReliableFragmentSendCounts.push_back(ReliableFragmentSendCount());
			recentReliableFragmentSendCounts.back().sendCount = sendCount;
			recentReliableFragmentSendCounts.back().size = fragmentSize;
			if (recentReliableFragmentSendCounts.size() > 40)
			{
				recentReliableFragmentSendCounts.erase(recentReliableFragmentSendCounts.begin());
			}
			averageReliableFragmentSendCount = 0.0f;
			const float weigth = 1.0f / recentReliableFragmentSendCounts.size();
			for (ReliableFragmentSendCount& recentReliableFragmentSendCount : recentReliableFragmentSendCounts)
			{
				averageReliableFragmentSendCount += weigth * float(recentReliableFragmentSendCount.sendCount);
			}

			// Add to total reliable fragment send counts
			const std::map<uint64_t, uint64_t>::iterator it = reliableFragmentSendCounters.find(sendCount);
			if (it != reliableFragmentSendCounters.end())
			{
				(*it).second++;
			}
			else
			{
				reliableFragmentSendCounters[sendCount] = 1;
			}
		}

		void Connection::increaseSendQuotaPerSecond()
		{
			LOCK_GUARD(lock, mutex, other);
			if (reliableSendQuotaPerSecondDirectionCounter < 0)
			{
				reliableSendQuotaPerSecondDirectionCounter = 0;
			}
			reliableSendQuotaPerSecondDirectionCounter++;
			const double maxSendQuotaPerSecond = 1024.0 * 1024.0 * 1024.0;
			const double multiplier = 1.0 + 0.05 * std::min(reliableSendQuotaPerSecondDirectionCounter, 20);
			sendQuotaPerSecond = std::min(maxSendQuotaPerSecond, sendQuotaPerSecond * multiplier);
		}

		void Connection::decreaseSendQuotaPerSecond()
		{
			LOCK_GUARD(lock, mutex, other);
			if (reliableSendQuotaPerSecondDirectionCounter > 0)
			{
				reliableSendQuotaPerSecondDirectionCounter = 0;
			}
			reliableSendQuotaPerSecondDirectionCounter--;
			const double minSendQuotaPerSecond = 1000.0;
			const double multiplier = 1.0 + 0.01 * std::max(reliableSendQuotaPerSecondDirectionCounter, -10);
			sendQuotaPerSecond = std::max(minSendQuotaPerSecond, sendQuotaPerSecond * multiplier);
		}

		void Connection::beginPathMaximumSegmentSizeDiscovery()
		{
			LOCK_GUARD(lock, mutex, other);
			maximumSegmentSize = defaultMaximumSegmentSize; // MSS must be reset
			pathMaximumSegmentSizeDiscovery.emplace(PathMaximumSegmentSizeDiscovery());
			sendNextPathMaximumSegmentSizeDiscoveryPacket();
			for (ReliablePacketOut& reliablePacketOut : reliablePacketSendQueue)
			{
				reliablePacketOut.unacknowledgedFragments.clear();
			}
		}

		void Connection::sendNextPathMaximumSegmentSizeDiscoveryPacket()
		{
			LOCK_GUARD(lock, mutex, other);
			se_assert(pathMaximumSegmentSizeDiscovery);

			if (pathMaximumSegmentSizeDiscovery->maxAcknowledged)
			{
				// End of iteration
				static int maxIterations = 10;
				if (pathMaximumSegmentSizeDiscovery->iterations++ < maxIterations)
				{
					// Re-iterate
					const uint16_t maxAcknowledged = *pathMaximumSegmentSizeDiscovery->maxAcknowledged;
					const int iterations = pathMaximumSegmentSizeDiscovery->iterations;
					pathMaximumSegmentSizeDiscovery.emplace(PathMaximumSegmentSizeDiscovery());
					pathMaximumSegmentSizeDiscovery->min = maxAcknowledged;
					pathMaximumSegmentSizeDiscovery->iterations = iterations;
					sendNextPathMaximumSegmentSizeDiscoveryPacket();
					return;
				}
			}
			else
			{
				// Determine the size of the next MSS discovery packet
				/*
					The maximum size of the spehs-level payload is uint16 max (65536) minus the IP header size.
					According to some person in the internet, the IP header is 28+ bytes for IPv4, and 48+ bytes for IPv6.
					When I tested this I got a boost asio error on send when trying anything above 65507 when using IPv4 (which would suggest a 29 byte IP header in my case).
				*/
				uint16_t size = 65507;
				if (pathMaximumSegmentSizeDiscovery->lastSentSize)
				{
					se_assert(*pathMaximumSegmentSizeDiscovery->lastSentSize >= pathMaximumSegmentSizeDiscovery->min);
					const uint16_t spread = *pathMaximumSegmentSizeDiscovery->lastSentSize - pathMaximumSegmentSizeDiscovery->min;
					size = (*pathMaximumSegmentSizeDiscovery->lastSentSize) - spread / 2;
				}

				if (!pathMaximumSegmentSizeDiscovery->lastSentSize || size != pathMaximumSegmentSizeDiscovery->lastSentSize)
				{
					PacketHeader packetHeader;
					packetHeader.protocolId = PacketHeader::spehsProtocolId;
					packetHeader.connectionId = connectionId;
					packetHeader.packetType = PacketHeader::PacketType::PathMaximumSegmentSizeDiscovery;
					WriteBuffer writeBuffer;
					writeBuffer.write(packetHeader);

					PathMaximumSegmentSizeDiscoveryPacket pathMaximumSegmentSizeDiscoveryPacket;
					pathMaximumSegmentSizeDiscoveryPacket.size = size;
					pathMaximumSegmentSizeDiscoveryPacket.extraPayload.resize(size - writeBuffer.getOffset() - sizeof(pathMaximumSegmentSizeDiscoveryPacket.size) - sizeof(uint32_t/*extraPayload size*/));
					writeBuffer.write(pathMaximumSegmentSizeDiscoveryPacket);

					const std::vector<boost::asio::const_buffer> sendBuffers
					{
						boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
					};
					se_assert(boost::asio::buffer_size(sendBuffers) == pathMaximumSegmentSizeDiscoveryPacket.size);
					sendPacketImpl(sendBuffers, LogSentBytesType::PathMaximumSegmentSizeDiscovery);
					pathMaximumSegmentSizeDiscovery->lastSendTime = time::now();
					pathMaximumSegmentSizeDiscovery->lastSentSize = size;
					return;
				}
			}
			
			// End
			pathMaximumSegmentSizeDiscovery.reset();
			DEBUG_LOG(1, "Path maximum segment size discovery finished. MSS: " + std::to_string(maximumSegmentSize));
		}

		Connection::Status Connection::getStatus() const
		{
			LOCK_GUARD(lock, mutex, other);
			return status;
		}

		boost::asio::ip::udp::endpoint Connection::getRemoteEndpoint() const
		{
			LOCK_GUARD(lock, mutex, other);
			return endpoint;
		}

		Port Connection::getLocalPort() const
		{
			LOCK_GUARD(lock, mutex, other);
			return socket->getLocalPort();
		}

		uint16_t Connection::getMaximumSegmentSize() const
		{
			return maximumSegmentSize;
		}

		time::Time Connection::getPing() const
		{
			LOCK_GUARD(lock, mutex, other);
			return estimatedRoundTripTime;
		}

		float Connection::getAverageReliableFragmentSendCount() const
		{
			LOCK_GUARD(lock, mutex, other);
			return averageReliableFragmentSendCount;
		}

		double Connection::getSendQuotaPerSecond() const
		{
			LOCK_GUARD(lock, mutex, other);
			return sendQuotaPerSecond;
		}

		double Connection::getReliableSendQuota() const
		{
			LOCK_GUARD(lock, mutex, other);
			return reliableSendQuota;
		}

		double Connection::getUnreliableSendQuota() const
		{
			LOCK_GUARD(lock, mutex, other);
			return reliableSendQuota;
		}

		Connection::SentBytes Connection::getSentBytes() const
		{
			LOCK_GUARD(lock, mutex, other);
			return sentBytes;
		}

		Connection::ReceivedBytes Connection::getReceivedBytes() const
		{
			LOCK_GUARD(lock, mutex, other);
			return receivedBytes;
		}

		bool Connection::hasReliableUnacknowledgedBytesInQueue(const uint64_t minBytes) const
		{
			LOCK_GUARD(lock, mutex, other);
			uint64_t bytes = 0ull;
			for (const ReliablePacketOut& reliablePacketOut : reliablePacketSendQueue)
			{
				if (!reliablePacketOut.delivered && reliablePacketOut.unacknowledgedFragments.empty())
				{
					bytes += uint64_t(reliablePacketOut.payload.size());
				}
				else
				{
					for (const ReliablePacketOut::UnacknowledgedFragment& unacknowledgedFragment : reliablePacketOut.unacknowledgedFragments)
					{
						bytes += uint64_t(unacknowledgedFragment.size);
					}
				}

				if (bytes >= minBytes)
				{
					return true;
				}
			}
			return false;
		}

		uint64_t Connection::getReliableUnacknowledgedBytesInQueue() const
		{
			LOCK_GUARD(lock, mutex, other);
			uint64_t bytes = 0ull;
			for (const ReliablePacketOut& reliablePacketOut : reliablePacketSendQueue)
			{
				if (!reliablePacketOut.delivered && reliablePacketOut.unacknowledgedFragments.empty())
				{
					bytes += uint64_t(reliablePacketOut.payload.size());
				}
				else
				{
					for (const ReliablePacketOut::UnacknowledgedFragment& unacknowledgedFragment : reliablePacketOut.unacknowledgedFragments)
					{
						bytes += uint64_t(unacknowledgedFragment.size);
					}
				}
			}
			return bytes;
		}

		uint64_t Connection::getReliableSentAcknowledgedBytesInQueue() const
		{
			LOCK_GUARD(lock, mutex, other);
			uint64_t bytes = 0ull;
			for (const ReliablePacketOut& reliablePacketOut : reliablePacketSendQueue)
			{
				for (const ReliablePacketOut::AcknowledgedFragment& acknowledgedFragment : reliablePacketOut.acknowledgedFragments)
				{
					bytes += uint64_t(acknowledgedFragment.size);
				}
			}
			return bytes;
		}

		uint64_t Connection::getReliableReceivedBytesInQueue() const
		{
			LOCK_GUARD(lock, mutex, other);
			uint64_t bytes = 0ull;
			for (const ReceivingReliablePacket& receivingReliablePacket : receivingReliablePackets)
			{
				bytes += uint64_t(receivingReliablePacket.payload.size());
			}
			return bytes;
		}

		uint64_t Connection::getReliableStreamOffsetSend() const
		{
			LOCK_GUARD(lock, mutex, other);
			return reliableStreamOffsetSend;
		}

		uint64_t Connection::getReliableStreamOffsetReceive() const
		{
			LOCK_GUARD(lock, mutex, other);
			return reliableStreamOffsetReceive;
		}

		uint64_t Connection::getSocketSendPacketCallCount() const
		{
			LOCK_GUARD(lock, mutex, other);
			return socketSendPacketCallCount;
		}

		std::map<uint64_t, uint64_t> Connection::getReliableFragmentSendCounters() const
		{
			LOCK_GUARD(lock, mutex, other);
			return reliableFragmentSendCounters;
		}

		void Connection::resetReliableFragmentSendCounters()
		{
			LOCK_GUARD(lock, mutex, other);
			reliableFragmentSendCounters.clear();
		}

		Connection::MutexTimes Connection::getAcquireMutexTimes() const
		{
			LOCK_GUARD(lock, mutex, other);
			return acquireMutexTimes;
		}

		Connection::MutexTimes Connection::getHoldMutexTimes() const
		{
			LOCK_GUARD(lock, mutex, other);
			return holdMutexTimes;
		}

		void Connection::resetMutexTimes()
		{
			LOCK_GUARD(lock, mutex, other);
			acquireMutexTimes = MutexTimes();
			holdMutexTimes = MutexTimes();
		}

		void Connection::connectToStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const Status, const Status)>& callback)
		{
			LOCK_GUARD(lock, mutex, other);
			scopedConnection = statusChangedSignal.connect(callback);
		}

		void Connection::setStatus(const Status newStatus, const std::lock_guard<std::recursive_mutex>& upperLock, const LockGuard<std::recursive_mutex>& lock)
		{
			/*
				Notes about deadlocks:
				By firing a signal we pass the execution to... somewhere. If this somewhere leads to ConnectionManager (which we must assume to happen sometimes), then we might find ourselves in a mutex deadlock:
				execution has stopped because some background thread is holding the ConnectionManager mutex. That background thread in turn is awaiting to acquire this connection's mutex.
				Thus we must acquire the upper management level mutex before passing execution to the signal.				
				When code reaches here, both upperMutex and mutex should be locked. To enforce this I've added the lock_guard parameters.
			*/
			if (newStatus != status)
			{
				const Status oldStatus = status;
				status = newStatus;
				statusChangedSignal(oldStatus, status);
			}
		}

		void Connection::setTimeoutEnabled(const bool value)
		{
			LOCK_GUARD(lock, mutex, other);
			timeoutEnabled = value;
		}

		bool Connection::getTimeoutEnabled() const
		{
			LOCK_GUARD(lock, mutex, other);
			return timeoutEnabled;
		}

		void Connection::setConnectionSimulationSettings(const ConnectionSimulationSettings& _connectionSimulationSettings)
		{
			LOCK_GUARD(lock, mutex, other);
			connectionSimulationSettings = _connectionSimulationSettings;
			if (connectionSimulationSettings.maximumSegmentSizeOutgoing > 0)
			{
				connectionSimulationSettings.maximumSegmentSizeOutgoing = std::max(defaultMaximumSegmentSize, connectionSimulationSettings.maximumSegmentSizeOutgoing);
				maximumSegmentSize = std::min(maximumSegmentSize, connectionSimulationSettings.maximumSegmentSizeOutgoing);
			}
		}

		ConnectionSimulationSettings Connection::getConnectionSimulationSettings() const
		{
			LOCK_GUARD(lock, mutex, other);
			return connectionSimulationSettings;
		}

		void Connection::setDebugLogLevel(const int level)
		{
			LOCK_GUARD(lock, mutex, other);
			debugLogLevel = level;
		}

		int Connection::getDebugLogLevel() const
		{
			LOCK_GUARD(lock, mutex, other);
			return debugLogLevel;
		}

		bool Connection::hasPendingOperations() const
		{
			LOCK_GUARD(lock, mutex, other);
			return reliablePacketSendQueue.size() > 0;
		}
	}
}
