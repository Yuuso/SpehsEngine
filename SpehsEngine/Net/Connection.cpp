#include "stdafx.h"
#include "SpehsEngine/Net/Connection.h"

#include "SpehsEngine/Net/Internal/InternalTypes.h"
#include "steam/isteamnetworkingutils.h"


namespace se
{
	namespace net
	{
		namespace
		{
			std::atomic<ConnectionId::ValueType> nextConnectionId = 1;
		}

		Connection::Connection(const ConnectionParameters& parameters)
			: name(parameters.name)
			, connectionId(nextConnectionId++)
			, establishmentType(parameters.establishmentType)
			, remoteEndpoint(parameters.remoteEndpoint)
			, localListeningPort(parameters.localListeningPort)
			, p2p(parameters.p2p)
			, status(parameters.status)
			, state(new ConnectionInternalState(parameters))
		{
			setSettingsImpl(settings, true);
			se_assert(state->steamNetConnection != k_HSteamNetConnection_Invalid);
		}

		Connection::~Connection()
		{
			if (state->steamNetConnection != k_HSteamNetConnection_Invalid)
			{
				disconnectImpl("~Connection()", true);
			}
		}

		void Connection::update()
		{
			if (!receivedPackets.empty() && state->receiveHandler)
			{
				std::vector<ReceivedPacket> receivedPackets2;
				receivedPackets2.swap(receivedPackets);
				for (size_t i = 0; i < receivedPackets2.size(); i++)
				{
					const ReceivedPacket& receivedPacket = receivedPackets2[i];
					BinaryReader binaryReader(receivedPacket.data.data(), receivedPacket.data.size());
					state->receiveHandler(binaryReader, receivedPacket.reliable);
					if (!state->receiveHandler)
					{
						// Push the rest of the packets back to the queue until a receive handler is specified
						const size_t beginIndex = i + 1;
						const size_t endIndex = receivedPackets2.size() - 1;
						const size_t count = endIndex - beginIndex;
						if (count > 0)
						{
							receivedPackets.resize(count);
							for (size_t p = 0; p < count; p++)
							{
								std::swap(receivedPackets[i], receivedPackets2[beginIndex + i]);
							}
						}
					}
				}
			}
		}

		bool Connection::sendPacket(const BinaryWriter& binaryWriter, const bool reliable)
		{
			const int flags = reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable;
			const EResult result = state->steamNetworkingSockets.SendMessageToConnection(state->steamNetConnection, binaryWriter.getData(), uint32_t(binaryWriter.getSize()), flags, nullptr);
			if (result == k_EResultOK)
			{
				if (reliable)
				{
					statistics.sentPacketsReliable++;
					statistics.sentBytesReliable += int64_t(binaryWriter.getSize());
				}
				else
				{
					statistics.sentPacketsUnreliable++;
					statistics.sentBytesUnreliable += int64_t(binaryWriter.getSize());
				}
				statistics.sentPacketsTotal++;
				statistics.sentBytesTotal += int64_t(binaryWriter.getSize());
				return true;
			}
			else
			{
				std::vector<char> buffer(10000);
				if (state->steamNetworkingSockets.GetDetailedConnectionStatus(state->steamNetConnection, buffer.data(), int(buffer.size())) == 0)
				{
					log::warning(se::formatString("Failed to send packet: %s", buffer.data()));
				}
				if (result != k_EResultRemoteDisconnect)
				{
					se_assert(!enableAssertOnSendFail && "Send failed");
				}
				return false;
			}
		}

		void Connection::disconnect(const std::string& reason)
		{
			disconnectImpl(reason, true);
			setStatus(Status::Disconnected);
		}

		void Connection::disconnectImpl(const std::string& reason, const bool enableLinger)
		{
			if (state->steamNetConnection != k_HSteamNetConnection_Invalid)
			{
				closeConnectionImpl(state->steamNetConnection, reason, enableLinger);
				state->closedSteamNetConnection = state->steamNetConnection; // Used later on by the ConnectionManager
				state->steamNetConnection = k_HSteamNetConnection_Invalid;
			}
		}

		void Connection::setStatus(const Status _status)
		{
			if (_status != status)
			{
				const Status oldStatus = status;
				status = _status;
				state->statusChangedSignal(oldStatus, status);
			}
		}

		void Connection::receivePacket(const void* data, const size_t size, const bool reliable)
		{
			// Track statistics
			statistics.receivedPacketsTotal++;
			statistics.receivedBytesTotal += int64_t(size);
			if (reliable)
			{
				statistics.receivedPacketsReliable++;
				statistics.receivedBytesReliable += int64_t(size);
			}
			else
			{
				statistics.receivedPacketsUnreliable++;
				statistics.receivedBytesUnreliable += int64_t(size);
			}

			// Process packet immediately?
			if (state->receiveHandler && receivedPackets.empty())
			{
				BinaryReader binaryReader((const uint8_t*)data, size);
				state->receiveHandler(binaryReader, reliable);
			}
			else
			{
				receivedPackets.push_back(ReceivedPacket());
				receivedPackets.back().reliable = reliable;
				receivedPackets.back().data.resize(size);
				memcpy(receivedPackets.back().data.data(), data, size);
			}
		}

		void Connection::setSettings(const Settings& _settings)
		{
			setSettingsImpl(_settings, false);
		}

		void Connection::setSettingsImpl(const Settings& _settings, const bool forceUpdate)
		{
			if (settings.sendBufferSize != _settings.sendBufferSize || forceUpdate)
			{
				se_assert(settings.sendBufferSize <= std::numeric_limits<uint32_t>::max());
				if (!SteamNetworkingUtils_Lib()->SetConnectionConfigValueInt32(state->steamNetConnection, k_ESteamNetworkingConfig_SendBufferSize, int32_t(_settings.sendBufferSize)))
				{
					log::error("Failed to set connection send buffer size.");
				}
			}
			if (settings.timeout != _settings.timeout || forceUpdate)
			{
				if (!SteamNetworkingUtils_Lib()->SetConnectionConfigValueInt32(state->steamNetConnection, k_ESteamNetworkingConfig_TimeoutConnected, int32_t(_settings.timeout.asMilliseconds())))
				{
					log::error("Failed to set connection timeout time.");
				}
			}
			settings = _settings;
		}

		time::Time Connection::getPing() const
		{
			SteamNetConnectionRealTimeStatus_t steamNetConnectionRealTimeStatus;
			SteamNetConnectionRealTimeLaneStatus_t steamNetConnectionRealTimeLaneStatus;
			if (state->steamNetworkingSockets.GetConnectionRealTimeStatus(state->steamNetConnection, &steamNetConnectionRealTimeStatus, 0, &steamNetConnectionRealTimeLaneStatus))
			{
				return time::fromMilliseconds(float(steamNetConnectionRealTimeStatus.m_nPing));
			}
			else
			{
				return time::fromMilliseconds(9999.9f);
			}
		}

		uint16_t Connection::getMaximumSegmentSize() const
		{
			return 1300; // internal value k_cbSteamNetworkingSocketsMaxUDPMsgLen
		}

		Connection::DetailedStatus Connection::getDetailedStatus() const
		{
			DetailedStatus detailedStatus;
			SteamNetConnectionRealTimeStatus_t steamNetConnectionRealTimeStatus;
			SteamNetConnectionRealTimeLaneStatus_t steamNetConnectionRealTimeLaneStatus;
			if (state->steamNetworkingSockets.GetConnectionRealTimeStatus(state->steamNetConnection, &steamNetConnectionRealTimeStatus, 0, &steamNetConnectionRealTimeLaneStatus))
			{
				detailedStatus.reliableBytesInSendQueue = steamNetConnectionRealTimeStatus.m_cbPendingReliable;
				detailedStatus.unreliableBytesInSendQueue = steamNetConnectionRealTimeStatus.m_cbPendingUnreliable;
				detailedStatus.sentUnackedReliableBytes = steamNetConnectionRealTimeStatus.m_cbSentUnackedReliable;
				detailedStatus.ping = time::fromMilliseconds(float(steamNetConnectionRealTimeStatus.m_nPing));
			}
			return detailedStatus;
		}

		void Connection::closeConnectionImpl(const uint32_t _steamNetConnection, const std::string& reason, const bool enableLinger)
		{
			if (SteamNetworkingSockets()->CloseConnection(_steamNetConnection, 0, reason.c_str(), enableLinger))
			{
				log::info("Steam net connection closed: " + std::to_string(uint32_t(_steamNetConnection)), log::TextColor::DARKGRAY);
			}
			else
			{
				log::error("Failed to close connection");
			}
		}

		void Connection::setReceiveHandler(const std::function<void(BinaryReader&, const bool)>& _receiveHandler)
		{
			state->receiveHandler = _receiveHandler;
		}

		void Connection::connectToStatusChangedSignal(ScopedConnection& scopedConnection, const std::function<void(const Status oldStatus, const Status newStatus)>& callback)
		{
			scopedConnection = state->statusChangedSignal.connect(callback);
		}
	}
}
