#include "stdafx.h"
#include "SpehsEngine/Net/Connection.h"

#include "SpehsEngine/Core/LockGuard.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/WriteBuffer.h"

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


/**///#pragma optimize("", off)

namespace se
{
	namespace net
	{
		SE_STRONG_INT(uint32_t, ProtocolId, 0u);
		ProtocolId spehsProtocolId(0x5E2070C0);

		enum class PacketType : uint8_t { connect, disconnect, heartbeat, mtuDiscovery };

		struct ConnectPacket
		{
			void write(se::WriteBuffer& writeBuffer) const
			{
			}
			bool read(se::ReadBuffer& readBuffer)
			{
				return true;
			}
		};

		struct DisconnectPacket
		{
			void write(se::WriteBuffer& writeBuffer) const
			{
			}
			bool read(se::ReadBuffer& readBuffer)
			{
				return true;
			}
		};

		struct HeartbeatPacket
		{
			void write(se::WriteBuffer& writeBuffer) const
			{
			}
			bool read(se::ReadBuffer& readBuffer)
			{
				return true;
			}
		};

		struct PathMTUDiscoveryPacket
		{
			void write(se::WriteBuffer& writeBuffer) const
			{
				se_write(writeBuffer, payload);
			}
			bool read(se::ReadBuffer& readBuffer)
			{
				se_read(readBuffer, payload);
				return true;
			}
			std::vector<uint8_t> payload;
		};

		struct PacketHeader
		{
			enum ControlBit : uint8_t
			{
				userData = 1 << 0,
				reliable = 1 << 1,
				endOfPayload = 1 << 2,
				acknowledgePacket = 1 << 3,
			};
			void write(se::WriteBuffer& writeBuffer) const
			{
				se_write(writeBuffer, protocolId);
				se_write(writeBuffer, controlBits);
				if (controlBits & ControlBit::reliable)
				{
					se_write(writeBuffer, streamOffset);
					se_write(writeBuffer, payloadTotalSize);
				}
				if (controlBits & ControlBit::acknowledgePacket)
				{
					se_write(writeBuffer, streamOffset);
					se_write(writeBuffer, receivedPayloadSize);
				}
			}
			bool read(se::ReadBuffer& readBuffer)
			{
				se_read(readBuffer, protocolId);
				se_read(readBuffer, controlBits);
				if (controlBits & ControlBit::reliable)
				{
					se_read(readBuffer, streamOffset);
					se_read(readBuffer, payloadTotalSize);
				}
				if (controlBits & ControlBit::acknowledgePacket)
				{
					se_read(readBuffer, streamOffset);
					se_read(readBuffer, receivedPayloadSize);
				}
				return true;
			}
			ProtocolId protocolId;
			uint8_t controlBits = 0u;
			uint64_t streamOffset = 0u; // TODO
			uint64_t payloadTotalSize = 0u;
			uint16_t receivedPayloadSize = 0u;
		};

		Connection::Connection(const boost::shared_ptr<SocketUDP2>& _socket, const boost::asio::ip::udp::endpoint& _endpoint,
			const EstablishmentType _establishmentType, const std::string& _debugName)
			: debugName(_debugName)
			, debugEndpoint(toString(_endpoint))
			, endpoint(_endpoint)
			, establishmentType(_establishmentType)
			, socket(_socket)
		{
			LOCK_GUARD(lock, mutex, other);
			reliablePacketSendQueue.emplace_back();
			reliablePacketSendQueue.back().userData = false;
			ConnectPacket connectPacket;
			WriteBuffer writeBuffer;
			writeBuffer.write(PacketType::connect);
			writeBuffer.write(connectPacket);
			writeBuffer.swap(reliablePacketSendQueue.back().payload);

#ifndef SE_FINAL_RELEASE // Disable timeouts in dev builds by default
			setTimeoutEnabled(false);
#endif
		}

		Connection::~Connection()
		{
			LOCK_GUARD(lock, mutex, other);
			se_assert(reliablePacketSendQueue.empty() || isConnecting());
		}

		void Connection::update()
		{
			SE_SCOPE_PROFILER(debugName);
			if (socket->isOpen())
			{
				deliverReceivedPackets();
			}
			else
			{
				setConnected(false);
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

			if (isConnected())
			{
				// Heartbeat
				LOCK_GUARD(lock, mutex, heartbeat);
				const se::time::Time now = time::now();
				static const time::Time reliableHeartbeatInterval = time::fromSeconds(1.0f);
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
				if (timeoutEnabled)
				{
					const time::Time timeoutTime = reliableHeartbeatInterval * 5;
					if (time::now() - lastReceiveTime >= timeoutTime)
					{
						setConnected(false);
					}
				}
			}
		}

		void Connection::processReceivedPackets()
		{
			SE_SCOPE_PROFILER(debugName);
			// Process received raw packets
			LOCK_GUARD(lock, mutex, processReceivedPackets);
			while (!receivedPackets.empty())
			{
				// Swap data and erase packet: receive handler might modify received packets vector!
				std::vector<uint8_t> data;
				data.swap(receivedPackets.front());
				receivedBytes += data.size();
				receivedPackets.erase(receivedPackets.begin());
				ReadBuffer readBuffer(data.data(), data.size());

				PacketHeader packetHeader;
				if (packetHeader.read(readBuffer))
				{
					if (packetHeader.protocolId == spehsProtocolId)
					{
						if (packetHeader.controlBits & PacketHeader::reliable)
						{
							if (packetHeader.controlBits & PacketHeader::acknowledgePacket)
							{
								acknowledgementReceiveHandler(packetHeader.streamOffset, packetHeader.receivedPayloadSize);
							}
							else
							{
								const bool userData = packetHeader.controlBits & PacketHeader::ControlBit::userData;
								const bool endOfPayload = packetHeader.controlBits & PacketHeader::ControlBit::endOfPayload;
								const uint16_t payloadSize = uint16_t(readBuffer.getBytesRemaining());
								reliableFragmentReceiveHandler(packetHeader.streamOffset, readBuffer[readBuffer.getOffset()], payloadSize, userData, endOfPayload, packetHeader.payloadTotalSize);
								sendPacketAcknowledgement(packetHeader.streamOffset, payloadSize);
							}
						}
						else
						{
							if (readBuffer.getBytesRemaining())
							{
								if (packetHeader.controlBits & PacketHeader::userData)
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
					else
					{
						DEBUG_LOG(1, "received packet header had unmatching protocol id.");
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

#ifndef SE_FINAL_RELEASE // Check that overlapping data matches
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

					if (insertIndex > 0)
					{
						if (checkMergeWithNext(insertIndex - 1))
						{
							insertIndex--;
						}
					}
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

				receivedReliableFragments.emplace_back();
				receivedReliableFragments.back().offset = reliableStreamOffset;
				receivedReliableFragments.back().userData = userData;
				receivedReliableFragments.back().endOfPayload = endOfPayload;
				receivedReliableFragments.back().data.resize(dataSize);
				memcpy(receivedReliableFragments.back().data.data(), dataPtr, dataSize);
				postInsert(receivedReliableFragments.size() - 1);
			}
			//else if (reliableStreamOffset == 0 && reliableStreamOffsetReceive > dataSize)
			//{
			//	//Connection was reset in between
			//	setConnected(false);
			//}
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
								if (reliablePacketSendQueue[p].unacknowledgedFragments[f].sendCount > 2 && getDebugLogLevel() >= 1)
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
				// Old acknowledgement
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
				const time::Time resendTime = estimatedRoundTripTime != time::Time::zero
					? estimatedRoundTripTime.value + estimatedRoundTripTime.value / 6
					: se::time::fromSeconds(1.0f / 10.0f);
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
							packetHeader.streamOffset = unacknowledgedFragment.offset;
							packetHeader.payloadTotalSize = reliablePacketOut.payload.size();
							packetHeader.controlBits |= PacketHeader::ControlBit::reliable;
							if (reliablePacketOut.userData)
							{
								packetHeader.controlBits |= PacketHeader::ControlBit::userData;
							}
							const bool endOfPayload = (unacknowledgedFragment.offset + unacknowledgedFragment.size) == (reliablePacketOut.payloadOffset + reliablePacketOut.payload.size());
							if (endOfPayload)
							{
								packetHeader.controlBits |= PacketHeader::ControlBit::endOfPayload;
							}
							WriteBuffer writeBuffer;
							writeBuffer.write(packetHeader);

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
				if (unreliablePacketSendQueue.front().userData)
				{
					packetHeader.controlBits |= PacketHeader::ControlBit::userData;
				}
				WriteBuffer writeBuffer;
				writeBuffer.write(packetHeader);
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
				while (!receivedReliableFragments.empty() && receivedReliableFragments.front().endOfPayload && receivedReliableFragments.front().offset == reliableStreamOffsetReceive)
				{
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
			else if (packetType == PacketType::connect)
			{
				ConnectPacket connectPacket;
				if (!readBuffer.read(connectPacket))
				{
					log::warning("Connection::spehsReceiveHandler: received invalid data.");
					return;
				}
				setConnected(true);
			}
			else if (packetType == PacketType::disconnect)
			{
				DisconnectPacket disconnectPacket;
				if (!readBuffer.read(disconnectPacket))
				{
					log::warning("Connection::spehsReceiveHandler: received invalid data.");
					return;
				}
				setConnected(false);
			}
			else
			{
				log::error("Connection::spehsReceiveHandler: received invalid data.");
				return;
			}
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
			if (connectionStatus == ConnectionStatus::connected)
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
#ifndef SE_FINAL_RELEASE
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
			packetHeader.controlBits |= PacketHeader::reliable;
			packetHeader.controlBits |= PacketHeader::acknowledgePacket;
			packetHeader.streamOffset = reliableStreamOffset;
			packetHeader.receivedPayloadSize = payloadSize;
			WriteBuffer writeBuffer;
			writeBuffer.write(packetHeader);
			const std::vector<boost::asio::const_buffer> sendBuffers
			{
				boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
			};
			sendPacketImpl(sendBuffers, false, false);
		}

		void Connection::stopConnecting()
		{
			LOCK_GUARD(lock, mutex, other);
			if (connectionStatus == ConnectionStatus::connecting)
			{
				connectionStatus = ConnectionStatus::disconnected;
			}
			else
			{
				se_assert(false && "Connection status is not connecting.");
			}
		}

		void Connection::disconnect()
		{
			LOCK_GUARD(lock, mutex, other);
			if (connectionStatus == ConnectionStatus::connected)
			{
				connectionStatus = ConnectionStatus::disconnecting;
				const uint64_t payloadOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + uint64_t(reliablePacketSendQueue.back().payload.size());
				reliablePacketSendQueue.emplace_back();
				reliablePacketSendQueue.back().userData = false;
				reliablePacketSendQueue.back().payloadOffset = payloadOffset;
				DisconnectPacket disconnectPacket;
				WriteBuffer writeBuffer;
				writeBuffer.write(PacketType::disconnect);
				writeBuffer.write(disconnectPacket);
				writeBuffer.swap(reliablePacketSendQueue.back().payload);
			}
			else
			{
				se_assert(false && "Connection status is not connecting.");
			}
		}

		void Connection::receivePacket(std::vector<uint8_t>& data)
		{
			LOCK_GUARD(lock, mutex, receivePacket);
#ifndef SE_FINAL_RELEASE
			if (simulatedPacketLossChanceIncoming > 0.0f && rng::weightedCoin(simulatedPacketLossChanceIncoming))
			{
				return;
			}
#endif
			if (data.size() > 0)
			{
				lastReceiveTime = time::now();
				receivedPackets.emplace_back();
				receivedPackets.back().swap(data);
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

		void Connection::setConnected(const bool value)
		{
			LOCK_GUARD(lock, mutex, other);
			const bool connected = isConnected();
			if (connected != value)
			{
				if (value)
				{
					connectionStatus = ConnectionStatus::connected;
					DEBUG_LOG(1, " connected.");
				}
				else
				{
					connectionStatus = ConnectionStatus::disconnected;
					reliableStreamOffsetSend = 0u;
					reliableStreamOffsetReceive = 0u;
					estimatedRoundTripTime = time::Time::zero;
					reliablePacketSendQueue.clear();
					unreliablePacketSendQueue.clear();
					receivedPackets.clear();
					receivedReliableFragments.clear();
					receivedReliablePackets.clear();
					receivedUnreliablePackets.clear();
					DEBUG_LOG(1, " disconnected.");
				}
				connectionStatusChangedSignal(value);
			}
		}

		bool Connection::isConnecting() const
		{
			LOCK_GUARD(lock, mutex, other);
			return connectionStatus == ConnectionStatus::connecting;
		}

		bool Connection::isConnected() const
		{
			LOCK_GUARD(lock, mutex, other);
			return connectionStatus == ConnectionStatus::connected;
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

		void Connection::connectToConnectionStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const bool)>& callback)
		{
			LOCK_GUARD(lock, mutex, other);
			scopedConnection = connectionStatusChangedSignal.connect(callback);
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

		void Connection::setSimulatedPacketLoss(const float chanceToDropIncoming, const float chanceToDropOutgoing)
		{
			LOCK_GUARD(lock, mutex, other);
			simulatedPacketLossChanceIncoming = chanceToDropIncoming;
			simulatedPacketLossChanceOutgoing = chanceToDropOutgoing;
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
