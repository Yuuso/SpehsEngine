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

/**/#pragma optimize("", off)

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

		struct PacketHeader
		{
			enum ControlBit : uint8_t
			{
				//connection
				connect = 1 << 0,
				disconnect = 1 << 1,

				//reliability
				endOfPayload = 1 << 5,
				acknowledgePacket = 1 << 6,
				reliable = 1 << 7,
			};
			void write(se::WriteBuffer& writeBuffer) const
			{
				se_write(writeBuffer, protocolId);
				se_write(writeBuffer, controlBits);
				if (controlBits & ControlBit::reliable)
				{
					se_write(writeBuffer, sequenceNumber);
				}
				if (controlBits & ControlBit::acknowledgePacket)
				{
					se_write(writeBuffer, receivedPayloadSize);
				}
			}
			bool read(se::ReadBuffer& readBuffer)
			{
				se_read(readBuffer, protocolId);
				se_read(readBuffer, controlBits);
				if (controlBits & ControlBit::reliable)
				{
					se_read(readBuffer, sequenceNumber);
				}
				if (controlBits & ControlBit::acknowledgePacket)
				{
					se_read(readBuffer, receivedPayloadSize);
				}
				return true;
			}
			ProtocolId protocolId;
			uint8_t controlBits = 0u;
			uint16_t sequenceNumber = 0u;
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
			reliablePacketSendQueue.emplace_back();
			reliablePacketSendQueue.back().type = ReliablePacketOut::Type::connect;
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
				receivedBytes += receivedPackets.front().size();
				ReadBuffer readBuffer(receivedPackets.front().data(), receivedPackets.front().size());
				PacketHeader packetHeader;
				if (packetHeader.read(readBuffer))
				{
					if (packetHeader.protocolId == spehsProtocolId)
					{
						if (packetHeader.controlBits & PacketHeader::disconnect)
						{
							setConnected(false);
							return;
						}

						if (packetHeader.controlBits & PacketHeader::reliable)
						{
							if (packetHeader.controlBits & PacketHeader::acknowledgePacket)
							{
								if (!reliablePacketSendQueue.empty())
								{
									if (packetHeader.sequenceNumber == sequenceNumber)
									{
										//Outgoing packet delivery confirmed
										sequenceNumber++;

										//Calculate RTT
										recentRoundTripTimes.push_back(time::now() - reliablePacketSendQueue.front().firstSendTime);
										if (recentRoundTripTimes.size() > 40)
										{
											recentRoundTripTimes.erase(recentRoundTripTimes.begin());
										}
										estimatedRoundTripTimeQueued = true;

										//Advance sent payload offset
										reliablePacketSendQueue.front().payloadSentOffset += size_t(packetHeader.receivedPayloadSize);
										se_assert(reliablePacketSendQueue.front().payloadSentOffset <= reliablePacketSendQueue.front().payload.size());
										if (reliablePacketSendQueue.front().payloadSentOffset == reliablePacketSendQueue.front().payload.size())
										{
											reliablePacketSendQueue.erase(reliablePacketSendQueue.begin());
										}
									}
								}
							}
							else
							{
								if (packetHeader.sequenceNumber == remoteSequenceNumber)
								{
									const uint16_t payloadSize = uint16_t(readBuffer.getBytesRemaining());

									//Add to received reliable bytes
									receivedBytesReliable += payloadSize;

									//Accumulate to the reliable receive buffer
									const size_t reliableReceiveBufferOffset = reliableReceiveBuffer.size();
									reliableReceiveBuffer.resize(reliableReceiveBuffer.size() + readBuffer.getBytesRemaining());
									memcpy(reliableReceiveBuffer.data() + reliableReceiveBufferOffset, readBuffer[readBuffer.getOffset()], readBuffer.getBytesRemaining());

									//Flush reliableReceiveBuffer?
									if (packetHeader.controlBits & PacketHeader::endOfPayload)
									{
										if (packetHeader.controlBits & PacketHeader::connect)
										{
											setConnected(true);
										}
										else
										{
											receivedReliablePackets.emplace_back();
											receivedReliablePackets.back().swap(reliableReceiveBuffer);
											reliableReceiveBuffer.clear();
										}
									}

									//Send receive acknowledgement
									sendPacketAcknowledgement(packetHeader.sequenceNumber, payloadSize);

									//Increment remote sequence number
									remoteSequenceNumber++;
								}
								else if (packetHeader.sequenceNumber == remoteSequenceNumber - 1)
								{
									//Received the previously received packet. The sender must not have gotten the confirmation packet (yet). Send it again.
									sendPacketAcknowledgement(packetHeader.sequenceNumber, previousReceivedPayloadSize);
								}
								else
								{
									//Received a really old packet. This one must have been off the line for a while. Don't mind it.
								}
							}
						}
						else if (readBuffer.getBytesRemaining())
						{
							receivedBytesUnreliable += readBuffer.getBytesRemaining();
							receivedUnreliablePackets.emplace_back();
							receivedUnreliablePackets.back().resize(readBuffer.getBytesRemaining());
							memcpy(receivedUnreliablePackets.back().data(), readBuffer[readBuffer.getOffset()], readBuffer.getBytesRemaining());
						}
						else
						{
							se::log::error("Received an empty unreliable packet.");
						}
					}
					else
					{
						DEBUG_LOG(1, "received packet header had unmatching protocol id.");
					}
				}

				receivedPackets.erase(receivedPackets.begin());
			}
		}

		void Connection::deliverReceivedPackets()
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

		void Connection::deliverOutgoingPackets()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (!reliablePacketSendQueue.empty() && time::now() - reliablePacketSendQueue.front().latestSendTime >= estimatedRoundTripTime)
			{
				PacketHeader packetHeader;
				packetHeader.protocolId = spehsProtocolId;
				packetHeader.sequenceNumber = sequenceNumber;
				packetHeader.controlBits |= PacketHeader::reliable;
				if (reliablePacketSendQueue.front().type == ReliablePacketOut::Type::userData)
				{
				}
				else if (reliablePacketSendQueue.front().type == ReliablePacketOut::Type::connect)
				{
					packetHeader.controlBits |= PacketHeader::connect;
				}
				else
				{
					se_assert(false && "Reliable packet type is not set.");
				}
				packetHeader.controlBits |= PacketHeader::endOfPayload;
				WriteBuffer writeBuffer;
				writeBuffer.write(packetHeader);
				if (writeBuffer.getSize() + reliablePacketSendQueue.front().payload.size() - reliablePacketSendQueue.front().payloadSentOffset > mtu)
				{
					//Packet is too large to fit the mtu limit. Disable end of payload bit and split the user data.
					packetHeader.controlBits = packetHeader.controlBits & ~PacketHeader::endOfPayload;
					writeBuffer = WriteBuffer();
					writeBuffer.write(packetHeader);
				}
				const std::vector<boost::asio::const_buffer> sendBuffers
				{
					boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
					boost::asio::const_buffer(reliablePacketSendQueue.front().payload.data() + reliablePacketSendQueue.front().payloadSentOffset,
					std::min(mtu, reliablePacketSendQueue.front().payload.size() - reliablePacketSendQueue.front().payloadSentOffset))
				};

				const bool firstSend = reliablePacketSendQueue.front().firstSendTime == time::Time::zero;

				sendPacketImpl(sendBuffers, firstSend, false);
				reliablePacketSendQueue.front().sendCount++;
				if (firstSend)
				{
					reliablePacketSendQueue.front().firstSendTime = time::now();
					reliablePacketSendQueue.front().latestSendTime = reliablePacketSendQueue.front().firstSendTime;
				}
			}

			while (!unreliablePacketSendQueue.empty())
			{
				PacketHeader packetHeader;
				packetHeader.protocolId = spehsProtocolId;
				WriteBuffer writeBuffer;
				writeBuffer.write(packetHeader);
				const std::vector<boost::asio::const_buffer> sendBuffers
				{
					boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
					boost::asio::const_buffer(unreliablePacketSendQueue.front().payload.data(), unreliablePacketSendQueue.front().payload.size()),
				};
				sendPacketImpl(sendBuffers, false, true);
				unreliablePacketSendQueue.erase(unreliablePacketSendQueue.begin());
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
			if (reliable)
			{
				reliablePacketSendQueue.emplace_back();
				reliablePacketSendQueue.back().type = ReliablePacketOut::Type::userData;
				reliablePacketSendQueue.back().payload.resize(writeBuffer.getSize());
				memcpy(reliablePacketSendQueue.back().payload.data(), writeBuffer.getData(), writeBuffer.getSize());
			}
			else
			{
				unreliablePacketSendQueue.emplace_back();
				unreliablePacketSendQueue.back().payload.resize(writeBuffer.getSize());
				memcpy(unreliablePacketSendQueue.back().payload.data(), writeBuffer.getData(), writeBuffer.getSize());
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

		void Connection::sendPacketAcknowledgement(const uint16_t sequenceNumber, const uint16_t payloadSize)
		{
			PacketHeader packetHeader;
			packetHeader.protocolId = spehsProtocolId;
			packetHeader.controlBits |= PacketHeader::reliable;
			packetHeader.controlBits |= PacketHeader::acknowledgePacket;
			packetHeader.sequenceNumber = sequenceNumber;
			packetHeader.receivedPayloadSize = payloadSize;
			WriteBuffer writeBuffer;
			writeBuffer.write(packetHeader);
			const std::vector<boost::asio::const_buffer> sendBuffers
			{
				boost::asio::const_buffer(writeBuffer.getData(), writeBuffer.getSize()),
			};
			sendPacketImpl(sendBuffers, false, false);
			previousReceivedPayloadSize = payloadSize;
		}

		void Connection::stopConnecting()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			connecting = false;
		}

		void Connection::disconnect()
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			PacketHeader packetHeader;
			packetHeader.protocolId = spehsProtocolId;
			packetHeader.controlBits |= PacketHeader::disconnect;
			WriteBuffer writeBuffer;
			writeBuffer.write(packetHeader);
			socket->sendPacket(writeBuffer, endpoint); // Send packet directly, immediately
			setConnected(false);
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
			return connecting;
		}

		bool Connection::isConnected() const
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return connected;
		}
		
		void Connection::setConnected(const bool value)
		{
			std::lock_guard<std::recursive_mutex> lock(mutex);
			if (connected != value)
			{
				connected = value;
				connectionStatusChangedSignal(connected);
				if (connected)
				{
					connecting = false;
					DEBUG_LOG(1, " connected.");
				}
				else
				{
					estimatedRoundTripTime = time::Time::zero;
					DEBUG_LOG(1, " disconnected.");
				}
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
				return receivedBytesReliable;
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
	}
}
