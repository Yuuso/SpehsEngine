#include "stdafx.h"
#include "SpehsEngine/Net/Connection.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

#define DEBUG_LOG(level, message) if (getDebugLogLevel() >= level) \
{ \
	se::log::info(debugName + "(" + getLocalPort().toString() + "): " + message); \
}

/**///#pragma optimize("", off)

namespace
{
	typedef uint16_t SizeType;
#ifdef SE_FINAL_RELEASE
	static const se::time::Time connectionTimeout = se::time::fromSeconds(5.0f);
#else
	static const se::time::Time connectionTimeout = se::time::fromSeconds(10000.0f);
#endif
}

namespace se
{
	namespace net
	{
		SE_STRONG_INT(uint32_t, ProtocolId, 0u);
		ProtocolId spehsProtocolId(0x5E2070C0);

		enum class PacketType { connect, disconnect };
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
			size_t streamOffset = 0u;
			size_t payloadTotalSize = 0u;
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
			std::lock_guard<std::recursive_mutex> lock(mutex);
			reliablePacketSendQueue.emplace_back();
			reliablePacketSendQueue.back().userData = false;
			ConnectPacket connectPacket;
			WriteBuffer writeBuffer;
			writeBuffer.write(PacketType::connect);
			writeBuffer.write(connectPacket);
			writeBuffer.swap(reliablePacketSendQueue.back().payload);
		}

		Connection::~Connection()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			se_assert(reliablePacketSendQueue.empty());
		}

		void Connection::update()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (socket->isOpen())
			{
				deliverReceivedPackets();
			}
			else
			{
				setConnected(false);
			}

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
					for (size_t i = 0; i < recentRoundTripTimes.size(); i++)
					{
						totalMilliseconds += recentRoundTripTimes[i].asMilliseconds();
					}
					estimatedRoundTripTime = time::fromMilliseconds(totalMilliseconds / float(recentRoundTripTimes.size()));
				}
			}
		}

		void Connection::processReceivedPackets()
		{
			//Process received raw packets
			std::lock_guard<std::recursive_mutex> lock(mutex);
			while (!receivedPackets.empty())
			{
				//Swap data and erase packet: receive handler might modify received packets vector!
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
									spehsReceiveHandler(ReadBuffer(readBuffer[readBuffer.getOffset()], readBuffer.getBytesRemaining()));
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

		void Connection::reliableFragmentReceiveHandler(const size_t reliableStreamOffset, const uint8_t* const dataPtr, const uint16_t dataSize, const bool userData, const bool endOfPayload, const size_t payloadTotalSize)
		{
			se_assert(dataSize > 0);
			if (reliableStreamOffset >= reliableStreamOffsetReceive)
			{
				auto postInsert = [&](size_t insertIndex)
				{
					auto checkMergeWithNext = [&](const size_t insertIndex)->bool
					{
						se_assert(insertIndex < receivedReliableFragments.size() - 1);
						//Check if fragment is to be merged with the next fragment
						if (!receivedReliableFragments[insertIndex].endOfPayload && insertIndex + 1 < receivedReliableFragments.size())
						{
							const size_t endOffset = receivedReliableFragments[insertIndex].offset + receivedReliableFragments[insertIndex].data.size();
							if (endOffset >= receivedReliableFragments[insertIndex + 1].offset)
							{
								const size_t overlappingSize = endOffset - receivedReliableFragments[insertIndex + 1].offset;

#ifndef SE_FINAL_RELEASE // Check that overlapping data matches
								for (size_t o = 0; o < overlappingSize; o++)
								{
									se_assert(receivedReliableFragments[insertIndex].data[receivedReliableFragments[insertIndex].data.size() - overlappingSize + o] == receivedReliableFragments[insertIndex + 1].data[o]);
								}
#endif

								//Copy next fragment's contents and erase it
								const size_t prevSize = receivedReliableFragments[insertIndex].data.size();
								const size_t mergedSize = prevSize + receivedReliableFragments[insertIndex + 1].data.size() - overlappingSize;
								receivedReliableFragments[insertIndex].data.resize(mergedSize);
								memcpy(
									&receivedReliableFragments[insertIndex].data[prevSize],
									&receivedReliableFragments[insertIndex + 1].data[overlappingSize],
									receivedReliableFragments[insertIndex + 1].data.size() - overlappingSize);
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
					if (insertIndex == 0 && receivedReliableFragments[insertIndex].endOfPayload)
					{
						se_assert(receivedReliableFragments[insertIndex].data.size() == payloadTotalSize);
					}
				};

				//Receive fragment
				for (size_t f = 0; f < receivedReliableFragments.size(); f++)
				{
					if (reliableStreamOffset < receivedReliableFragments[f].offset)
					{
						//Insert before
						ReceivedFragment& receivedFragment = *receivedReliableFragments.insert(receivedReliableFragments.begin() + f, ReceivedFragment());
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
						//Received fragment overlaps with receivedReliableFragments[f]'s coverage
						//Check if any new data was received
						if (reliableStreamOffset + dataSize > receivedReliableFragments[f].offset + receivedReliableFragments[f].data.size())
						{
							se_assert(!receivedReliableFragments[f].endOfPayload);
							const size_t newBytes = reliableStreamOffset + dataSize - receivedReliableFragments[f].offset - receivedReliableFragments[f].data.size();
							const size_t prevSize = receivedReliableFragments[f].data.size();
							receivedReliableFragments[f].data.resize(prevSize + newBytes);
							memcpy(&receivedReliableFragments[f].data[prevSize], dataPtr + (dataSize - newBytes), newBytes);
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

		void Connection::acknowledgementReceiveHandler(const size_t reliableStreamOffset, const uint16_t payloadSize)
		{
			if (reliableStreamOffset >= reliableStreamOffsetSend)
			{
				size_t offset = reliableStreamOffsetSend;
				for (size_t p = 0; p < reliablePacketSendQueue.size(); p++)
				{
					se_assert(reliableStreamOffset >= offset);
					if (reliableStreamOffset < offset + reliablePacketSendQueue[p].payload.size())
					{
						//Acknowledgement belongs to this packet, check fragments
						for (size_t f = 0; f < reliablePacketSendQueue[p].unacknowledgedFragments.size(); f++)
						{
							if (reliablePacketSendQueue[p].unacknowledgedFragments[f].offset == reliableStreamOffset &&
								reliablePacketSendQueue[p].unacknowledgedFragments[f].size == payloadSize)
							{
								//Calculate RTT
								recentRoundTripTimes.push_back(time::now() - reliablePacketSendQueue[p].unacknowledgedFragments[f].firstSendTime);
								if (recentRoundTripTimes.size() > 40)
								{
									recentRoundTripTimes.erase(recentRoundTripTimes.begin());
								}
								estimatedRoundTripTimeQueued = true;

								//Add to recentReliableFragmentSendCounts
								recentReliableFragmentSendCounts.push_back(
									std::make_pair(reliablePacketSendQueue[p].unacknowledgedFragments[f].sendCount, reliablePacketSendQueue[p].unacknowledgedFragments[f].size));
								if (recentReliableFragmentSendCounts.size() > 40)
								{
									recentReliableFragmentSendCounts.erase(recentReliableFragmentSendCounts.begin());
								}
								averageReliableFragmentSendCount = 0.0f;
								const float weigth = 1.0f / recentReliableFragmentSendCounts.size();
								for (size_t a = 0; a < recentReliableFragmentSendCounts.size(); a++)
								{
									averageReliableFragmentSendCount += weigth * float(recentReliableFragmentSendCounts[a].first);
								}

								//Add to acknowledged fragments
								reliablePacketSendQueue[p].acknowledgedFragments.push_back(ReliablePacketOut::AcknowledgedFragment());
								reliablePacketSendQueue[p].acknowledgedFragments.back().offset = reliableStreamOffset;
								reliablePacketSendQueue[p].acknowledgedFragments.back().size = payloadSize;

								//Remove from unacknowledged fragments
								reliablePacketSendQueue[p].unacknowledgedFragments.erase(reliablePacketSendQueue[p].unacknowledgedFragments.begin() + f);

								//Remove all unacknowledged fragments that have been delivered through other fragments
								//TODO, only happens when MTU changes on the fly

								//Check if the complete payload has been delivered
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
						//Not in this packet
						se_assert(p + 1 < reliablePacketSendQueue.size());
						offset += reliablePacketSendQueue[p].payload.size();
					}
				}
			}
			else
			{
				//Old acknowledgement
			}
		}

		void Connection::deliverOutgoingPackets()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);

			//Remove delivered reliable packets from the send queue
			while (!reliablePacketSendQueue.empty() && reliablePacketSendQueue.front().delivered)
			{
				reliableStreamOffsetSend += reliablePacketSendQueue.front().payload.size();
				reliablePacketSendQueue.erase(reliablePacketSendQueue.begin());
			}
			
			int remainingSendQuota = 4000; // TODO: define the send quota to avoid congestion
			mtu = 1400;//TEMP...
			for (size_t q = 0; q < reliablePacketSendQueue.size() && remainingSendQuota > 0; q++)
			{
				ReliablePacketOut& reliablePacketOut = reliablePacketSendQueue[q];
				
				//Generate unacknowledged fragments
				if (!reliablePacketOut.delivered && reliablePacketOut.unacknowledgedFragments.empty())
				{
					size_t offset = reliablePacketOut.payloadOffset;
					const size_t offsetEnd = reliablePacketOut.payloadOffset + reliablePacketOut.payload.size();
					size_t remaining = reliablePacketOut.payload.size();
					while (remaining > 0)
					{
						const uint16_t fragmentSize = uint16_t(std::min(remaining, mtu));
						reliablePacketOut.unacknowledgedFragments.emplace_back();
						reliablePacketOut.unacknowledgedFragments.back().offset = offsetEnd - remaining;
						reliablePacketOut.unacknowledgedFragments.back().size = fragmentSize;
						remaining -= fragmentSize;
					}
				}

				//Keep (re)sending unacknowledged fragments
				const time::Time resendTime = estimatedRoundTripTime != time::Time::zero
					? estimatedRoundTripTime.value + estimatedRoundTripTime.value / 6
					: se::time::fromSeconds(1.0f / 10.0f);
				const time::Time now = time::now();
				for (size_t f = 0; f < reliablePacketOut.unacknowledgedFragments.size() && remainingSendQuota > 0; f++)
				{
					ReliablePacketOut::UnacknowledgedFragment &unacknowledgedFragment = reliablePacketOut.unacknowledgedFragments[f];
					if (now - unacknowledgedFragment.latestSendTime >= resendTime)
					{
						if (unacknowledgedFragment.size <= mtu)
						{
							//Packet is valid for (re)send
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

							const size_t internalPayloadOffset = unacknowledgedFragment.offset - reliablePacketOut.payloadOffset;
							const std::vector<boost::asio::const_buffer> sendBuffers
							{
								boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
								boost::asio::const_buffer(reliablePacketOut.payload.data() + internalPayloadOffset, unacknowledgedFragment.size)
							};

							sendPacketImpl(sendBuffers, true, false);
							remainingSendQuota -= writeBuffer.getSize();

							if (unacknowledgedFragment.sendCount++ == 0)
							{
								unacknowledgedFragment.firstSendTime = time::now();
							}
							unacknowledgedFragment.latestSendTime = time::now();
						}
						else
						{
							//Packet is too big to fit the (changed) mtu -> split
							const size_t originalOffset = unacknowledgedFragment.offset;
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

			remainingSendQuota = 1; // TEMP
			for (size_t i = 0; i < unreliablePacketSendQueue.size() && remainingSendQuota > 0; i++)
			{
				PacketHeader packetHeader;
				packetHeader.protocolId = spehsProtocolId;
				if (unreliablePacketSendQueue[i].userData)
				{
					packetHeader.controlBits |= PacketHeader::ControlBit::userData;
				}
				WriteBuffer writeBuffer;
				writeBuffer.write(packetHeader);
				const std::vector<boost::asio::const_buffer> sendBuffers
				{
					boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
					boost::asio::const_buffer(unreliablePacketSendQueue[i].payload.data(), unreliablePacketSendQueue[i].payload.size()),
				};
				sendPacketImpl(sendBuffers, false, true);
				remainingSendQuota -= writeBuffer.getSize() + unreliablePacketSendQueue[i].payload.size();
			}
			//Drop all remaining unreliable packets to make way for reliable traffic...
			unreliablePacketSendQueue.clear();

			if (connectionStatus == ConnectionStatus::disconnecting && reliablePacketSendQueue.empty())
			{
				PacketHeader packetHeader;
				packetHeader.protocolId = spehsProtocolId;
				DisconnectPacket disconnectPacket;
				WriteBuffer writeBuffer;
				writeBuffer.write(packetHeader);
				writeBuffer.write(PacketType::disconnect);
				writeBuffer.write(disconnectPacket);
				const std::vector<boost::asio::const_buffer> sendBuffers
				{
					boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
				};
				sendPacketImpl(sendBuffers, false, true); // Send packet directly, immediately, unreliably.
				setConnected(false);
			}
		}

		void Connection::deliverReceivedPackets()
		{
			//Check if all data for the next packet has been received
			{
				std::lock_guard<std::recursive_mutex> lock(mutex);
				while (!receivedReliableFragments.empty() && receivedReliableFragments.front().endOfPayload)
				{
					reliableStreamOffsetReceive += receivedReliableFragments.front().data.size();
					if (receivedReliableFragments.front().userData)
					{
						receivedReliablePackets.emplace_back();
						receivedReliablePackets.back().swap(receivedReliableFragments.front().data);
					}
					else
					{
						spehsReceiveHandler(ReadBuffer(receivedReliableFragments.front().data.data(), receivedReliableFragments.front().data.size()));
					}
					receivedReliableFragments.erase(receivedReliableFragments.begin());
				}
			}

			if (isConnected())
			{
				//Deliver received reliable packets
				while (true)
				{
					std::vector<uint8_t> data;
					std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> handler;
					{
						std::lock_guard<std::recursive_mutex> lock(mutex);
						if (receivedReliablePackets.empty() || !receiveHandler)
						{
							if (receivedReliablePackets.size() > 1000)
							{
								log::warning(debugName + std::to_string(receivedReliablePackets.size()) + " received reliable packets awaiting.");
							}
							break;
						}
						else
						{
							//Copy contents to non-mutex protected memory
							data.swap(receivedReliablePackets.front());
							handler = receiveHandler;
							receivedReliablePackets.erase(receivedReliablePackets.begin());
						}
					}
					ReadBuffer readBuffer(data.data(), data.size());
					handler(readBuffer, endpoint, true);
				}

				//Deliver received unreliable packets
				while (true)
				{
					std::vector<uint8_t> data;
					std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> handler;
					{
						std::lock_guard<std::recursive_mutex> lock(mutex);
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
							//Copy contents to non-mutex protected memory
							data.swap(receivedUnreliablePackets.front());
							handler = receiveHandler;
							receivedUnreliablePackets.erase(receivedUnreliablePackets.begin());
						}
					}
					ReadBuffer readBuffer(data.data(), data.size());
					handler(readBuffer, endpoint, false);
				}
			}
		}

		void Connection::spehsReceiveHandler(ReadBuffer& readBuffer)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			PacketType packetType;
			if (!readBuffer.read(packetType))
			{
				log::warning("Connection::spehsReceiveHandler: received invalid data.");
				return;
			}

			if (packetType == PacketType::connect)
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
				log::warning("Connection::spehsReceiveHandler: received invalid data.");
				return;
			}
		}

		void Connection::setReceiveHandler(const std::function<void(ReadBuffer&, const boost::asio::ip::udp::endpoint&, const bool)> callback)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			receiveHandler = callback;		
		}

		void Connection::sendPacket(const WriteBuffer& writeBuffer, const bool reliable)
		{
			if (writeBuffer.isEmpty())
			{
				return;
			}
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (connectionStatus == ConnectionStatus::connected)
			{
				if (reliable)
				{
					const size_t payloadOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + reliablePacketSendQueue.back().payload.size();
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
			std::lock_guard<std::recursive_mutex> lock(mutex);
#ifndef SE_FINAL_RELEASE
			if (rng::weightedCoin(simulatedPacketLossChanceOutgoing))
			{
				sentBytes += bufferSize;
				if (logReliable)
				{
					sentBytesReliable += bufferSize;
				}
				if (logUnreliable)
				{
					sentBytesUnreliable += bufferSize;
				}
				return;
			}
#endif
			if (socket->sendPacket(buffers, endpoint))
			{
				sentBytes += bufferSize;
				if (logReliable)
				{
					sentBytesReliable += bufferSize;
				}
				if (logUnreliable)
				{
					sentBytesUnreliable += bufferSize;
				}
				lastSendTime = time::now();
			}
		}

		void Connection::sendPacketAcknowledgement(const size_t reliableStreamOffset, const uint16_t payloadSize)
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
			std::lock_guard<std::recursive_mutex> lock(mutex);
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
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (connectionStatus == ConnectionStatus::connected)
			{
				connectionStatus = ConnectionStatus::disconnecting;

				//Add a disconnect packet to the reliable send queue
				PacketHeader packetHeader;
				packetHeader.protocolId = spehsProtocolId;
				DisconnectPacket disconnectPacket;
				WriteBuffer writeBuffer;
				writeBuffer.write(packetHeader);
				writeBuffer.write(PacketType::disconnect);
				writeBuffer.write(disconnectPacket);
				const size_t payloadOffset = reliablePacketSendQueue.empty() ? reliableStreamOffsetSend : reliablePacketSendQueue.back().payloadOffset + reliablePacketSendQueue.back().payload.size();
				reliablePacketSendQueue.push_back(ReliablePacketOut(false, payloadOffset, writeBuffer.getData(), writeBuffer.getSize()));
			}
			else
			{
				se_assert(false && "Connection status is not connecting.");
			}
		}

		void Connection::receivePacket(std::vector<uint8_t>& data)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
#ifndef SE_FINAL_RELEASE
			if (rng::weightedCoin(simulatedPacketLossChanceIncoming))
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

		bool Connection::isConnecting() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return connectionStatus == ConnectionStatus::connecting;
		}

		bool Connection::isConnected() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return connectionStatus == ConnectionStatus::connected;
		}
		
		void Connection::setConnected(const bool value)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
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

		boost::asio::ip::udp::endpoint Connection::getRemoteEndpoint() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return endpoint;
		}

		Port Connection::getLocalPort() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return socket->getLocalPort();
		}

		time::Time Connection::getPing() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return estimatedRoundTripTime;
		}

		float Connection::getAverageReliableFragmentSendCount() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return averageReliableFragmentSendCount;
		}

		size_t Connection::getSentBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return sentBytes;
		}

		size_t Connection::getSentBytes(const bool reliable) const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (reliable)
			{
				return sentBytesReliable;
			}
			else
			{
				return sentBytesUnreliable;
			}
		}

		size_t Connection::getReceivedBytes() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return receivedBytes;
		}

		size_t Connection::getReceivedBytes(const bool reliable) const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (reliable)
			{
				return reliableStreamOffsetReceive;
			}
			else
			{
				return receivedBytesUnreliable;
			}
		}

		void Connection::connectToConnectionStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const bool)>& callback)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			scopedConnection = connectionStatusChangedSignal.connect(callback);
		}

		void Connection::setSimulatedPacketLoss(const float chanceToDropIncoming, const float chanceToDropOutgoing)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			simulatedPacketLossChanceIncoming = chanceToDropIncoming;
			simulatedPacketLossChanceOutgoing = chanceToDropOutgoing;
		}

		void Connection::setDebugLogLevel(const int level)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			debugLogLevel = level;
		}

		int Connection::getDebugLogLevel() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return debugLogLevel;
		}

		bool Connection::hasPendingOperations() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return reliablePacketSendQueue.size() > 0;
		}
	}
}
