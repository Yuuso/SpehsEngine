#include "stdafx.h"
#include "SpehsEngine/Net/Connection.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/LockGuard.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/PrecompiledInclude.h"
#pragma optimize("", off) // nocommit
#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
	debugSelfLog.push_back(std::string(message)); \
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


/**///#pragma optimize("", off)

namespace se
{
	namespace net
	{
		ProtocolId spehsProtocolId(0x5E2070C0);
		const time::Time reliableHeartbeatInterval = time::fromSeconds(1.0f);
		const time::Time timeoutTime = reliableHeartbeatInterval * 5;

		enum class PacketType : uint8_t
		{
			connect,
			disconnect,
			heartbeat,
			mtuDiscovery,
		};

		Connection::Connection(const boost::shared_ptr<SocketUDP2>& _socket, const boost::asio::ip::udp::endpoint& _endpoint, const ConnectionId _connectionId,
			const EstablishmentType _establishmentType, const std::string_view _debugName)
			: debugName(_debugName)
			, debugEndpoint(toString(_endpoint))
			, endpoint(_endpoint)
			, connectionId(_connectionId)
			, establishmentType(_establishmentType)
			, socket(_socket)
		{
			LOCK_GUARD(lock, mutex, other);
			reliablePacketSendQueue.emplace_back();
			reliablePacketSendQueue.back().userData = false;
			ConnectPacket connectPacket;
			connectPacket.connectionId = _connectionId;
			connectPacket.debugName = _debugName;
			WriteBuffer writeBuffer;
			writeBuffer.write(PacketType::connect);
			writeBuffer.write(connectPacket);
			writeBuffer.swap(reliablePacketSendQueue.back().payload);

#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE // Disable timeouts in dev builds by default
			setTimeoutEnabled(false);
#endif
		}

		Connection::~Connection()
		{
			LOCK_GUARD(lock, mutex, other);
			disconnect();
		}

		void Connection::update(const time::Time timeoutDeltaTime)
		{
			SE_SCOPE_PROFILER(debugName);
			if (socket->isOpen())
			{
				deliverReceivedPackets();
			}
			else if (getStatus() != Status::Disconnected)
			{
				disconnectImpl(false);
			}

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
			}

			if (getStatus() == Status::Connected)
			{
				// Heartbeat
				LOCK_GUARD(lock, mutex, heartbeat);
				const se::time::Time now = time::now();
				if (now - lastQueueHeartbeatTime >= reliableHeartbeatInterval &&
					now - lastSendTimeReliable >= reliableHeartbeatInterval)
				{
					lastQueueHeartbeatTime = now;

					HeartbeatPacket heartbeatPacket;
					WriteBuffer writeBuffer;
					se_write(writeBuffer, PacketType::heartbeat);
					se_write(writeBuffer, heartbeatPacket);

					const uint64_t payloadOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + reliablePacketSendQueue.back().payload.size();
					reliablePacketSendQueue.emplace_back();
					reliablePacketSendQueue.back().userData = false;
					reliablePacketSendQueue.back().payloadOffset = payloadOffset;
					writeBuffer.swap(reliablePacketSendQueue.back().payload);
				}

				// Timeout disconnection
				timeoutCountdown -= timeoutDeltaTime;
				if (timeoutEnabled && timeoutCountdown <= time::Time::zero)
				{
					disconnect();
				}
			}
		}

		void Connection::processReceivedRawPackets()
		{
			SE_SCOPE_PROFILER(debugName);

			// Process received raw packets
			LOCK_GUARD(lock, mutex, processReceivedRawPackets);

			// Simulated raw packet reordering
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
			if (simulatedPacketChanceReordering > 0.0f)
			{
				if (receivedRawPackets.size() < 2)
				{
					// We do not process received raw packets until 2+ have arrived so that we can give the arrived packet a chance to be reordered
					return;
				}
				for (size_t i = 1; i < receivedRawPackets.size(); i++)
				{
					if (rng::weightedCoin(simulatedPacketChanceReordering))
					{
						// Swith to the previous packet
						receivedRawPackets[i - 1].swap(receivedRawPackets[i]);
					}
				}
			}
#endif

			while (!receivedRawPackets.empty())
			{
				// Swap data and erase packet: receive handler might modify received packets vector!
				std::vector<uint8_t> data;
				data.swap(receivedRawPackets.front());
				receivedBytes += data.size();
				receivedRawPackets.erase(receivedRawPackets.begin());
				ReadBuffer readBuffer(data.data(), data.size());

				PacketHeader packetHeader;
				if (packetHeader.read(readBuffer, debugName))
				{
					if (packetHeader.protocolId == spehsProtocolId)
					{
						if (remoteConnectionId)
						{
							if (packetHeader.connectionId == remoteConnectionId)
							{
								if (checkBit(packetHeader.controlBits, PacketHeader::ControlBit::Reliable))
								{
									if (checkBit(packetHeader.controlBits, PacketHeader::ControlBit::AcknowledgePacket))
									{
										acknowledgementReceiveHandler(packetHeader.streamOffset, packetHeader.receivedPayloadSize);
									}
									else
									{
										const bool userData = checkBit(packetHeader.controlBits, PacketHeader::ControlBit::UserData);
										const bool endOfPayload = checkBit(packetHeader.controlBits, PacketHeader::ControlBit::EndOfPayload);
										const uint16_t payloadSize = uint16_t(readBuffer.getBytesRemaining());
										reliableFragmentReceiveHandler(packetHeader.streamOffset, readBuffer[readBuffer.getOffset()], payloadSize, userData, endOfPayload, packetHeader.payloadTotalSize);
										sendPacketAcknowledgement(packetHeader.streamOffset, payloadSize);
									}
								}
								else
								{
									if (readBuffer.getBytesRemaining())
									{
										if (checkBit(packetHeader.controlBits, PacketHeader::ControlBit::UserData))
										{
											receivedBytesUnreliable += readBuffer.getBytesRemaining();
											receivedUnreliablePackets.emplace_back();
											receivedUnreliablePackets.back().resize(readBuffer.getBytesRemaining());
											memcpy(receivedUnreliablePackets.back().data(), readBuffer[readBuffer.getOffset()], readBuffer.getBytesRemaining());
										}
										else
										{
											ReadBuffer readBuffer2(readBuffer[readBuffer.getOffset()], readBuffer.getBytesRemaining());
											spehsReceiveHandler(readBuffer2);
										}
									}
									else
									{
										se::log::error("Received an empty unreliable packet.");
									}
								}
							}
							else if (time::now() - connectionEstablishedTime > time::fromSeconds(5.0f)
								&& time::now() - lastReceiveTime > time::fromSeconds(5.0f))
							{
								DEBUG_LOG(1, "received packet header had unmatching connection id. Current connection isn't responsive. Terminating connection...");
								disconnect();
							}
							else
							{
								DEBUG_LOG(1, "received packet header had unmatching connection id. Ignoring...");
							}
						}
						else
						{
							// We are currently expecting only the connect packet, everything else will be discarded.
							// NOTE: Keep in mind that there might be packets from the previous (or even the next?) connection from this endpoint.
							if (checkBit(packetHeader.controlBits, PacketHeader::ControlBit::Reliable) &&
								!checkBit(packetHeader.controlBits, PacketHeader::ControlBit::UserData))
							{
								ReadBuffer readBuffer2(readBuffer[readBuffer.getOffset()], readBuffer.getBytesRemaining());
								attemptToProcessConnectPacket(readBuffer2);
							}
						}
					}
					else
					{
						DEBUG_LOG(2, "received packet header had unmatching protocol id.");
					}
				}
			}
		}

		void Connection::reliableFragmentReceiveHandler(const uint64_t reliableStreamOffset, const uint8_t* const dataPtr, const uint16_t dataSize, const bool userData, const bool endOfPayload, const uint64_t payloadTotalSize)
		{
			se_assert(dataSize > 0);
			if (reliableStreamOffset >= reliableStreamOffsetReceive)
			{
				auto postInsert = [&](size_t insertIndex)
				{
					auto checkMergeWithNext = [&](const size_t insertIndex)->bool
					{
						se_assert(insertIndex < receivedReliableFragments.size() - 1);
						// Check if fragment is to be merged with the next fragment
						if (!receivedReliableFragments[insertIndex].endOfPayload && insertIndex + 1 < receivedReliableFragments.size())
						{
							const uint64_t endOffset = receivedReliableFragments[insertIndex].offset + uint64_t(receivedReliableFragments[insertIndex].data.size());
							if (endOffset >= receivedReliableFragments[insertIndex + 1].offset)
							{
								const uint64_t overlappingSize = endOffset - receivedReliableFragments[insertIndex + 1].offset;

#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE // Check that overlapping data matches
								for (uint64_t o = 0; o < overlappingSize; o++)
								{
									se_assert(receivedReliableFragments[insertIndex].data[size_t(uint64_t(receivedReliableFragments[insertIndex].data.size()) - overlappingSize + o)] == receivedReliableFragments[insertIndex + 1].data[size_t(o)]);
								}
#endif

								// Copy next fragment's contents and erase it
								const size_t prevSize = receivedReliableFragments[insertIndex].data.size();
								const size_t mergedSize = prevSize + receivedReliableFragments[insertIndex + 1].data.size() - size_t(overlappingSize);
								receivedReliableFragments[insertIndex].data.resize(mergedSize);
								memcpy(
									&receivedReliableFragments[insertIndex].data[prevSize],
									&receivedReliableFragments[insertIndex + 1].data[size_t(overlappingSize)],
									size_t(uint64_t(receivedReliableFragments[insertIndex + 1].data.size()) - overlappingSize));
								receivedReliableFragments[insertIndex].endOfPayload = receivedReliableFragments[insertIndex + 1].endOfPayload;
								receivedReliableFragments.erase(receivedReliableFragments.begin() + insertIndex + 1);
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
					if (insertIndex + 1 < receivedReliableFragments.size())
					{
						checkMergeWithNext(insertIndex);
					}

					if (reliableStreamOffset == reliableStreamOffsetReceive && receivedReliableFragments[insertIndex].endOfPayload)
					{
						se_assert(insertIndex == 0 && "reliableStreamOffsetReceive points to the begin, but insertion happened elsewhere?");
						se_assert(receivedReliableFragments[insertIndex].data.size() == payloadTotalSize);
					}
				};

				// Receive fragment
				for (size_t f = 0; f < receivedReliableFragments.size(); f++)
				{
					if (reliableStreamOffset < receivedReliableFragments[f].offset)
					{
						// Insert before
						ReceivedReliableFragment& receivedFragment = *receivedReliableFragments.insert(receivedReliableFragments.begin() + f, ReceivedReliableFragment());
						receivedFragment.userData = userData;
						receivedFragment.endOfPayload = endOfPayload;
						receivedFragment.offset = reliableStreamOffset;
						receivedFragment.data.resize(dataSize);
						memcpy(receivedFragment.data.data(), dataPtr, dataSize);
						postInsert(f);
						return;
					}
					else if (reliableStreamOffset < receivedReliableFragments[f].offset + receivedReliableFragments[f].data.size())
					{
						// Received fragment overlaps with receivedReliableFragments[f]'s coverage
						// Check if any new data was received
						if (reliableStreamOffset + dataSize > receivedReliableFragments[f].offset + receivedReliableFragments[f].data.size())
						{
							se_assert(!receivedReliableFragments[f].endOfPayload);
							const uint64_t newBytes = reliableStreamOffset + dataSize - receivedReliableFragments[f].offset - receivedReliableFragments[f].data.size();
							const uint64_t prevSize = receivedReliableFragments[f].data.size();
							const uint64_t newSize = prevSize + newBytes;
							se_assert(newSize <= std::numeric_limits<size_t>::max() && "New size is larger than max std::vector capacity.");
							receivedReliableFragments[f].data.resize(size_t(newSize));
							memcpy(&receivedReliableFragments[f].data[size_t(prevSize)], dataPtr + (dataSize - newBytes), size_t(newBytes));
							receivedReliableFragments[f].endOfPayload = endOfPayload;
							postInsert(f);
						}
						return;
					}
				}

				const uint64_t backReliableStreamOffset = receivedReliableFragments.empty() ? 0u : receivedReliableFragments.back().offset + uint64_t(receivedReliableFragments.back().data.size());
				se_assert(reliableStreamOffset >= backReliableStreamOffset);

				receivedReliableFragments.emplace_back();
				receivedReliableFragments.back().offset = reliableStreamOffset;
				receivedReliableFragments.back().userData = userData;
				receivedReliableFragments.back().endOfPayload = endOfPayload;
				receivedReliableFragments.back().data.resize(dataSize);
				memcpy(receivedReliableFragments.back().data.data(), dataPtr, dataSize);
				postInsert(receivedReliableFragments.size() - 1);
			}
		}

		void Connection::acknowledgementReceiveHandler(const uint64_t reliableStreamOffset, const uint16_t payloadSize)
		{
			SE_SCOPE_PROFILER(debugName);
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
								reliablePacketSendQueue[p].acknowledgedFragments.push_back(ReliablePacketOut::AcknowledgedFragment());
								reliablePacketSendQueue[p].acknowledgedFragments.back().offset = reliableStreamOffset;
								reliablePacketSendQueue[p].acknowledgedFragments.back().size = payloadSize;

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
									if (reliablePacketSendQueue[p].payloadOffset == 0)
									{
										debugSelfLog.push_back("r0 delivered 1/2");
										//if (debugName == "Server XanaduClient")
										//{
										//	time::Time nocommit = time::now();
										//	if (nocommit == time::Time::zero)
										//		log::info("nocommit");
										//}
									}
								}

								break;
							}
						}

						break;
					}
					else
					{
						// Not in this packet
						se_assert(p + 1 < reliablePacketSendQueue.size()); // TODO?: asserted here once
						offset += reliablePacketSendQueue[p].payload.size();
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
			while (!reliablePacketSendQueue.empty() && reliablePacketSendQueue.front().delivered)
			{
				if (reliablePacketSendQueue.front().payloadOffset == 0)
				{
					debugSelfLog.push_back("r0 delivered 2/2");
				}
				reliableStreamOffsetSend += reliablePacketSendQueue.front().payload.size();
				reliablePacketSendQueue.erase(reliablePacketSendQueue.begin());
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
				const uint64_t newSendQuota = uint64_t(std::ceil(sendQuotaPerSecond * secondsSinceReplenish));
				if (newSendQuota >= 10)
				{
					const uint64_t newReliableSendQuota = newSendQuota / 2;
					const uint64_t newUnreliableSendQuota = newSendQuota - newReliableSendQuota;
					reliableSendQuota += newReliableSendQuota;
					unreliableSendQuota += newUnreliableSendQuota;
					lastSendQuotaReplenishTimestamp = now;
					//if (rng::weightedCoin(0.05f))
					//{
					//	log::info("(random sample) Send quota added: " + std::to_string(newSendQuota));
					//}
				}
				else
				{
					// Program is running too fast... let's wait a while longer before replenishing
				}
			}

			// If reliable/unreliable is empty, dump quota to the other. If both are empty, release all so that quota does not stack over time.
			if (reliablePacketSendQueue.empty() && unreliablePacketSendQueue.empty())
			{
				reliableSendQuota = 0u;
				unreliableSendQuota = 0u;
			}
			else if (reliablePacketSendQueue.empty())
			{
				unreliableSendQuota += reliableSendQuota;
				reliableSendQuota = 0u;
			}
			else if (unreliablePacketSendQueue.empty())
			{
				reliableSendQuota += unreliableSendQuota;
				unreliableSendQuota = 0u;
			}

			// By default do not request more send quota
			moreSendQuotaRequested = false;

			// Reliable send queue
			for (ReliablePacketOut& reliablePacketOut : reliablePacketSendQueue)
			{
				// Generate unacknowledged fragments
				if (!reliablePacketOut.delivered && reliablePacketOut.unacknowledgedFragments.empty())
				{
					const uint64_t offsetEnd = reliablePacketOut.payloadOffset + reliablePacketOut.payload.size();
					uint64_t remaining = reliablePacketOut.payload.size();
					while (remaining > 0)
					{
						const uint16_t fragmentSize = uint16_t(std::min(remaining, uint64_t(maximumSegmentSize)));
						reliablePacketOut.unacknowledgedFragments.emplace_back();
						reliablePacketOut.unacknowledgedFragments.back().offset = offsetEnd - remaining;
						reliablePacketOut.unacknowledgedFragments.back().size = fragmentSize;
						remaining -= fragmentSize;
					}
				}

				// Keep (re)sending unacknowledged fragments
				const time::Time capResendTime = se::time::fromSeconds(0.5f); // Do not wait longer than this to resend
				const time::Time resendTime = std::min(capResendTime,
					estimatedRoundTripTime != time::Time::zero
					? estimatedRoundTripTime.value + estimatedRoundTripTime.value / 6
					: se::time::fromSeconds(1.0f / 10.0f));
				for (size_t f = 0; f < reliablePacketOut.unacknowledgedFragments.size(); f++)
				{
					ReliablePacketOut::UnacknowledgedFragment &unacknowledgedFragment = reliablePacketOut.unacknowledgedFragments[f];
					if (now - unacknowledgedFragment.latestSendTime >= resendTime)
					{
						if (unacknowledgedFragment.size <= maximumSegmentSize)
						{
							// Packet is valid for (re)send
							PacketHeader packetHeader;
							packetHeader.protocolId = spehsProtocolId;
							packetHeader.connectionId = connectionId;
							packetHeader.streamOffset = unacknowledgedFragment.offset;
							packetHeader.payloadTotalSize = reliablePacketOut.payload.size();
							enableBit(packetHeader.controlBits, PacketHeader::ControlBit::Reliable);
							if (reliablePacketOut.userData)
							{
								enableBit(packetHeader.controlBits, PacketHeader::ControlBit::UserData);
							}
							const bool endOfPayload = (unacknowledgedFragment.offset + unacknowledgedFragment.size) == (reliablePacketOut.payloadOffset + reliablePacketOut.payload.size());
							if (endOfPayload)
							{
								enableBit(packetHeader.controlBits, PacketHeader::ControlBit::EndOfPayload);
							}
							WriteBuffer writeBuffer;
							packetHeader.write(writeBuffer, debugEndpoint);

							if (writeBuffer.getSize() + unacknowledgedFragment.size <= reliableSendQuota)
							{
								const uint64_t internalPayloadOffset = unacknowledgedFragment.offset - reliablePacketOut.payloadOffset;
								const std::vector<boost::asio::const_buffer> sendBuffers
								{
									boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
									boost::asio::const_buffer(reliablePacketOut.payload.data() + internalPayloadOffset, unacknowledgedFragment.size)
								};

								sendPacketImpl(sendBuffers, true, false);
								reliableSendQuota -= writeBuffer.getSize();

								if (unacknowledgedFragment.sendCount++ == 0)
								{
									unacknowledgedFragment.firstSendTime = time::now();
								}
								unacknowledgedFragment.latestSendTime = time::now();
							}
							else
							{
								// Not enough send quota. Break, we want to buffer up send quota to send this fragment asap.
								moreSendQuotaRequested = true;
								break;
							}
						}
						else
						{
							// Packet is too big to fit the (changed?) mss -> split
							const uint64_t originalOffset = unacknowledgedFragment.offset;
							const uint16_t size1 = unacknowledgedFragment.size / 2;
							const uint16_t size2 = unacknowledgedFragment.size - size1;
							ReliablePacketOut::UnacknowledgedFragment &unacknowledgedFragment2 =
								(*reliablePacketOut.unacknowledgedFragments.insert(reliablePacketOut.unacknowledgedFragments.begin() + f + 1, ReliablePacketOut::UnacknowledgedFragment()));
							unacknowledgedFragment = ReliablePacketOut::UnacknowledgedFragment();
							unacknowledgedFragment.offset = originalOffset;
							unacknowledgedFragment.size = size1;
							unacknowledgedFragment2.offset = originalOffset + size1;
							unacknowledgedFragment2.size = size2;
							f--; // Redo this iteration
						}
					}
				}
			}

			// Unreliable send queue
			while (!unreliablePacketSendQueue.empty())
			{
				PacketHeader packetHeader;
				packetHeader.protocolId = spehsProtocolId;
				packetHeader.connectionId = connectionId;
				if (unreliablePacketSendQueue.front().userData)
				{
					enableBit(packetHeader.controlBits, PacketHeader::ControlBit::UserData);
				}
				WriteBuffer writeBuffer;
				packetHeader.write(writeBuffer, debugEndpoint);
				if (writeBuffer.getSize() + unreliablePacketSendQueue.front().payload.size() <= unreliableSendQuota)
				{
					const std::vector<boost::asio::const_buffer> sendBuffers
					{
						boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
						boost::asio::const_buffer(unreliablePacketSendQueue.front().payload.data(), unreliablePacketSendQueue.front().payload.size()),
					};
					sendPacketImpl(sendBuffers, false, true);
					unreliableSendQuota -= writeBuffer.getSize() + unreliablePacketSendQueue.front().payload.size();
					unreliablePacketSendQueue.erase(unreliablePacketSendQueue.begin());
				}
				else
				{
					moreSendQuotaRequested = true;
					break;
				}
			}

			// Drop all unreliable packets that are getting stale
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

		void Connection::deliverReceivedPackets()
		{
			SE_SCOPE_PROFILER(debugName);
			// Check if all data for the next packet has been received
			{
				LOCK_GUARD(lock, mutex, deliverReceivedPackets);
				// nocommit: reliableStreamOffsetReceive is 0 when it should be 4, received reliable packets don't get processed
				while (!receivedReliableFragments.empty() && receivedReliableFragments.front().endOfPayload && receivedReliableFragments.front().offset == reliableStreamOffsetReceive)
				{
					if (reliableStreamOffsetReceive == 0)
						debugSelfLog.push_back("r0 received 2/2");
					se_assert(receivedReliableFragments.front().data.size() > 0);
					reliableStreamOffsetReceive += receivedReliableFragments.front().data.size();
					receivedReliablePackets.emplace_back();
					receivedReliablePackets.back().data.swap(receivedReliableFragments.front().data);
					receivedReliablePackets.back().userData = receivedReliableFragments.front().userData;
					receivedReliableFragments.erase(receivedReliableFragments.begin());
				}
			}

			// Deliver received reliable packets
			while (true)
			{
				std::vector<uint8_t> data;
				std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> handler;
				{
					LOCK_GUARD(lock, mutex, deliverReceivedPackets);
					if (!receivedReliablePackets.empty())
					{
						if (receivedReliablePackets.front().userData)
						{
							if (receiveHandler)
							{
								handler = receiveHandler;
								data.swap(receivedReliablePackets.front().data);
								receivedReliablePackets.erase(receivedReliablePackets.begin());
							}
							else
							{
								// Must wait until a receive handler is specified
								if (receivedReliablePackets.size() > 1000)
								{
									log::warning(debugName + std::to_string(receivedReliablePackets.size()) + " received reliable packets awaiting.");
								}
								break;
							}
						}
						else
						{
							handler = std::bind(&Connection::spehsReceiveHandler, this, std::placeholders::_1);
							data.swap(receivedReliablePackets.front().data);
							receivedReliablePackets.erase(receivedReliablePackets.begin());
						}
					}
				}
				if (handler)
				{
					SE_SCOPE_PROFILER("handler");
					se_assert(data.size() > 0);
					ReadBuffer readBuffer(data.data(), data.size());
					handler(readBuffer, endpoint, true);
				}
				else
				{
					se_assert(data.empty());
					break;
				}
			}

			// Deliver received unreliable packets
			while (true)
			{
				std::vector<uint8_t> data;
				std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> handler;
				{
					LOCK_GUARD(lock, mutex, deliverReceivedPackets);
					if (receivedUnreliablePackets.empty())
					{
						break;
					}
					else if (!receiveHandler)
					{
						DEBUG_LOG(1, std::to_string(receivedUnreliablePackets.size()) + " received unrealiable packets were discarded because no receive handler is set.");
						receivedUnreliablePackets.clear();
						break;
					}
					else
					{
						// Copy contents to non-mutex protected memory
						data.swap(receivedUnreliablePackets.front());
						handler = receiveHandler;
						receivedUnreliablePackets.erase(receivedUnreliablePackets.begin());
					}
				}
				ReadBuffer readBuffer(data.data(), data.size());
				handler(readBuffer, endpoint, false);
			}
		}

		void Connection::spehsReceiveHandler(ReadBuffer& readBuffer)
		{
			LOCK_GUARD(lock, mutex, spehsReceiveHandler);
			PacketType packetType;
			if (!readBuffer.read(packetType))
			{
				log::warning("Connection::spehsReceiveHandler: received invalid data.");
				return;
			}

			if (packetType == PacketType::heartbeat)
			{
				// Do nothing
			}
			else if (packetType == PacketType::disconnect)
			{
				DisconnectPacket disconnectPacket;
				if (!readBuffer.read(disconnectPacket))
				{
					log::warning("Connection::spehsReceiveHandler: received invalid data.");
					return;
				}

				if (getStatus() != Status::Disconnected)
				{
					// Disconnect without sending a packet in return
					disconnectImpl(false);
				}
			}
			else if (packetType == PacketType::connect)
			{
				// Ignore. The connect packet is only handled once when its expected (attemptToProcessConnectPacket).
			}
			else
			{
				log::error("Connection::spehsReceiveHandler: received invalid data.");
				return;
			}
		}

		void Connection::attemptToProcessConnectPacket(ReadBuffer& readBuffer)
		{
			se_assert(getStatus() == Status::Connecting);
			LOCK_GUARD(lock, mutex, spehsReceiveHandler);
			PacketType packetType;
			if (!readBuffer.read(packetType))
			{
				return;
			}

			if (packetType != PacketType::connect)
			{
				return;
			}

			ConnectPacket connectPacket;
			if (!readBuffer.read(connectPacket))
			{
				log::warning("Connection::attemptToProcessConnectPacket: packet type is connect but contents could not be read.");
				return;
			}

			if (!connectPacket.connectionId)
			{
				log::warning("Connection::attemptToProcessConnectPacket: connect packet does not define connection id.");
				return;
			}

			remoteConnectionId = connectPacket.connectionId;
			remoteDebugName = connectPacket.debugName;
			DEBUG_LOG(1, "connected.");
			setStatus(Status::Connected);
		}

		void Connection::setReceiveHandler(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> callback)
		{
			LOCK_GUARD(lock, mutex, other);
			receiveHandler = callback;
		}

		void Connection::sendPacket(const WriteBuffer& writeBuffer, const bool reliable)
		{
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
					const uint64_t payloadOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + uint64_t(reliablePacketSendQueue.back().payload.size());
					reliablePacketSendQueue.push_back(ReliablePacketOut(true, payloadOffset, writeBuffer.getData(), writeBuffer.getSize()));
				}
				else
				{
					unreliablePacketSendQueue.push_back(UnreliablePacketOut(writeBuffer.getData(), writeBuffer.getSize(), true));
				}
			}
			else
			{
				se_assert(false && "Connection is not in the connected state. Cannot send packet.");
			}
		}

		void Connection::sendPacketImpl(const std::vector<boost::asio::const_buffer>& buffers, const bool logReliable, const bool logUnreliable)
		{
			const size_t bufferSize = boost::asio::buffer_size(buffers);

			LOCK_GUARD(lock, mutex, sendPacketImpl);
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
			if (simulatedPacketLossChanceOutgoing > 0.0f && rng::weightedCoin(simulatedPacketLossChanceOutgoing))
			{
				sentBytes += uint64_t(bufferSize);
				if (logReliable)
				{
					sentBytesReliable += uint64_t(bufferSize);
				}
				if (logUnreliable)
				{
					sentBytesUnreliable += uint64_t(bufferSize);
				}
				return;
			}
#endif
			if (socket->sendPacket(buffers, endpoint))
			{
				sentBytes += uint64_t(bufferSize);
				if (logReliable)
				{
					sentBytesReliable += uint64_t(bufferSize);
				}
				if (logUnreliable)
				{
					sentBytesUnreliable += uint64_t(bufferSize);
				}
				lastSendTime = time::now();
				if (logReliable)
				{
					lastSendTimeReliable = lastSendTime;
				}
			}
		}

		void Connection::sendPacketAcknowledgement(const uint64_t reliableStreamOffset, const uint16_t payloadSize)
		{
			PacketHeader packetHeader;
			packetHeader.protocolId = spehsProtocolId;
			packetHeader.connectionId = connectionId;
			enableBit(packetHeader.controlBits, PacketHeader::ControlBit::Reliable);
			enableBit(packetHeader.controlBits, PacketHeader::ControlBit::AcknowledgePacket);
			packetHeader.streamOffset = reliableStreamOffset;
			packetHeader.receivedPayloadSize = payloadSize;
			WriteBuffer writeBuffer;
			packetHeader.write(writeBuffer, debugEndpoint);
			const std::vector<boost::asio::const_buffer> sendBuffers
			{
				boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
			};
			sendPacketImpl(sendBuffers, false, false);
			if (packetHeader.streamOffset == 0)
				debugSelfLog.push_back("r0 received 1/2"); // NOTE: packetHeader.streamOffset==0 triggering earlier than this may be the ack packet
		}

		void Connection::disconnect()
		{
			disconnectImpl(true);
		}

		void Connection::disconnectImpl(const bool sendDisconnectPacket)
		{
			LOCK_GUARD(lock, mutex, other);
			if (status == Status::Disconnected)
			{
				return;
			}

			// nocommit
			if (debugName == "XanaduServer: Incoming connection")
			{
				debugSelfLog.push_back("Xanadu client disconnected.");
			}
			else if (debugName == "Client XanaduClient")
			{
				debugSelfLog.push_back("Xanadu client disconnected.");
			}

			if (sendDisconnectPacket)
			{
				// Disconnect packet is sent immediately and unreliably. We do not want to wait for acks.
				WriteBuffer writeBuffer;
				PacketHeader packetHeader;
				packetHeader.protocolId = spehsProtocolId;
				packetHeader.connectionId = connectionId;
				packetHeader.write(writeBuffer, debugEndpoint);
				writeBuffer.write(PacketType::disconnect);
				DisconnectPacket disconnectPacket;
				writeBuffer.write(disconnectPacket);
				const std::vector<boost::asio::const_buffer> sendBuffers
				{
					boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize())
				};
				sendPacketImpl(sendBuffers, false, true);
			}

			remoteConnectionId = ConnectionId();
			reliableStreamOffsetSend = 0u;
			reliableStreamOffsetReceive = 0u;
			estimatedRoundTripTime = time::Time::zero;
			reliablePacketSendQueue.clear();
			unreliablePacketSendQueue.clear();
			receivedRawPackets.clear();
			receivedReliableFragments.clear();
			receivedReliablePackets.clear();
			receivedUnreliablePackets.clear();
			DEBUG_LOG(1, "disconnected.");
			setStatus(Status::Disconnected);
		}

		void Connection::receiveRawPacket(std::vector<uint8_t>& data)
		{
			LOCK_GUARD(lock, mutex, receivePacket);
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
			if (simulatedPacketLossChanceIncoming > 0.0f && rng::weightedCoin(simulatedPacketLossChanceIncoming))
			{
				return;
			}
#endif
			if (data.size() > 0)
			{
				lastReceiveTime = time::now();
				timeoutCountdown = timeoutTime;
				receivedRawPackets.emplace_back();
				receivedRawPackets.back().swap(data);
			}
		}

		void Connection::reliableFragmentTransmitted(const uint64_t sendCount, const uint16_t fragmentSize)
		{
			// Add to recentReliableFragmentSendCounts
			congestionAvoidanceState.recentReliableFragmentSendCounts.push_back(CongestionAvoidanceState::ReliableFragmentSendCount());
			congestionAvoidanceState.recentReliableFragmentSendCounts.back().sendCount = sendCount;
			congestionAvoidanceState.recentReliableFragmentSendCounts.back().size = fragmentSize;
			if (congestionAvoidanceState.recentReliableFragmentSendCounts.size() > 40)
			{
				congestionAvoidanceState.recentReliableFragmentSendCounts.erase(congestionAvoidanceState.recentReliableFragmentSendCounts.begin());
			}
			averageReliableFragmentSendCount = 0.0f;
			const float weigth = 1.0f / congestionAvoidanceState.recentReliableFragmentSendCounts.size();
			for (CongestionAvoidanceState::ReliableFragmentSendCount& recentReliableFragmentSendCount : congestionAvoidanceState.recentReliableFragmentSendCounts)
			{
				averageReliableFragmentSendCount += weigth * float(recentReliableFragmentSendCount.sendCount);
			}

			// Add to total reliable fragment send counts
			const std::map<uint64_t, uint64_t>::iterator it = congestionAvoidanceState.reliableFragmentSendCounters.find(sendCount);
			if (it != congestionAvoidanceState.reliableFragmentSendCounters.end())
			{
				(*it).second++;
			}
			else
			{
				congestionAvoidanceState.reliableFragmentSendCounters[sendCount] = 1;
			}

			double sendQuotaGrowthIncrease = 0.0;
			double sendQuotaGrowthFactor = 1.0;
			if (sendCount > 1u)
			{
				// Resend count is high, send less
				sendQuotaGrowthFactor = 0.999f;
			}
			else
			{
				// No resends, raise send quota
				if (moreSendQuotaRequested)
				{
					sendQuotaGrowthFactor = 1.001; // 1.01;
				}
			}

			if (sendQuotaGrowthFactor != 1.0)
			{
				se_assert(sendQuotaGrowthFactor != 0.0);
				static const double minSendQuotaPerSecond = 1000.0;
				sendQuotaPerSecond = std::max(sendQuotaGrowthFactor * sendQuotaPerSecond + sendQuotaGrowthIncrease, minSendQuotaPerSecond);
				//if (rng::weightedCoin(0.02f))
					//log::info(debugName + ": sendQuota " + (sendQuotaGrowthFactor > 1.0 ? "increased" : "decreased") + ": " + se::toString(float(sendQuotaPerSecond * 8.0 / 10000000), 6) + " Kb/s afsc: " + std::to_string(averageReliableFragmentSendCount));
			}

			congestionAvoidanceState.prevSendQuotaGrowthFactor = sendQuotaGrowthFactor;
			congestionAvoidanceState.prevReliableStreamOffsetSend = reliableStreamOffsetSend;
			congestionAvoidanceState.reEvaluationTimestamp = time::now();
		}

		Connection::Status Connection::getStatus() const
		{
			LOCK_GUARD(lock, mutex, other);
			return status;
		}

		bool Connection::isConnected() const
		{
			LOCK_GUARD(lock, mutex, other);
			return status == Status::Connected;
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

		uint64_t Connection::getSentBytes() const
		{
			LOCK_GUARD(lock, mutex, other);
			return sentBytes;
		}

		uint64_t Connection::getSentBytes(const bool reliable) const
		{
			LOCK_GUARD(lock, mutex, other);
			if (reliable)
			{
				return sentBytesReliable;
			}
			else
			{
				return sentBytesUnreliable;
			}
		}

		uint64_t Connection::getReceivedBytes() const
		{
			LOCK_GUARD(lock, mutex, other);
			return receivedBytes;
		}

		uint64_t Connection::getReceivedBytes(const bool reliable) const
		{
			LOCK_GUARD(lock, mutex, other);
			if (reliable)
			{
				return reliableStreamOffsetReceive;
			}
			else
			{
				return receivedBytesUnreliable;
			}
		}

		std::map<uint64_t, uint64_t> Connection::getReliableFragmentSendCounters() const
		{
			LOCK_GUARD(lock, mutex, other);
			return congestionAvoidanceState.reliableFragmentSendCounters;
		}

		void Connection::resetReliableFragmentSendCounters()
		{
			LOCK_GUARD(lock, mutex, other);
			congestionAvoidanceState.reliableFragmentSendCounters.clear();
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

		void Connection::setStatus(const Status newStatus)
		{
			LOCK_GUARD(lock, mutex, other);
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

		void Connection::setSimulatedPacketLoss(const float chanceToDropIncoming, const float chanceToDropOutgoing, const float chanceToReorderReceivedPacket)
		{
			LOCK_GUARD(lock, mutex, other);
			simulatedPacketLossChanceIncoming = chanceToDropIncoming;
			simulatedPacketLossChanceOutgoing = chanceToDropOutgoing;
			simulatedPacketChanceReordering = chanceToReorderReceivedPacket;
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
