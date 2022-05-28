#include "stdafx.h"
#include "SpehsEngine/Net/Connection2.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/PrecompiledInclude.h"
#include "steam/isteamnetworkingutils.h"


namespace se
{
	namespace net
	{
		namespace
		{
			std::atomic<ConnectionId::ValueType> nextConnectionId = 1;
		}

		Connection2::Connection2(const ConstructorParameters& constructorParameters)
			: name(constructorParameters.name)
			, connectionId(nextConnectionId++)
			, establishmentType(constructorParameters.establishmentType)
			, remoteEndpoint(constructorParameters.remoteEndpoint)
			, localListeningPort(constructorParameters.localListeningPort)
			, steamNetworkingSockets(*constructorParameters.steamNetworkingSockets)
			, steamNetConnection(constructorParameters.steamNetConnection)
			, steamListenSocket(constructorParameters.steamListenSocket)
			, p2p(constructorParameters.p2p)
			, status(constructorParameters.status)
		{
			setSettingsImpl(settings, true);
			se_assert(steamNetConnection != k_HSteamNetConnection_Invalid);
		}

		Connection2::~Connection2()
		{
			if (steamNetConnection != k_HSteamNetConnection_Invalid)
			{
				disconnectImpl("~Connection2()", false);
			}
		}

		void Connection2::update()
		{
			if (!receivedPackets.empty() && receiveHandler)
			{
				std::vector<ReceivedPacket> receivedPackets2;
				receivedPackets2.swap(receivedPackets);
				for (size_t i = 0; i < receivedPackets2.size(); i++)
				{
					const ReceivedPacket& receivedPacket = receivedPackets2[i];
					ReadBuffer readBuffer(receivedPacket.data.data(), receivedPacket.data.size());
					receiveHandler(readBuffer, receivedPacket.reliable);
					if (!receiveHandler)
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

		bool Connection2::sendPacket(const WriteBuffer& writeBuffer, const bool reliable)
		{
			const int flags = reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable;
			const EResult result = steamNetworkingSockets.SendMessageToConnection(steamNetConnection, writeBuffer.getData(), uint32_t(writeBuffer.getSize()), flags, nullptr);
			if (result == k_EResultOK)
			{
				if (reliable)
				{
					statistics.sentPacketsReliable++;
					statistics.sentBytesReliable += int64_t(writeBuffer.getSize());
				}
				else
				{
					statistics.sentPacketsUnreliable++;
					statistics.sentBytesUnreliable += int64_t(writeBuffer.getSize());
				}
				statistics.sentPacketsTotal++;
				statistics.sentBytesTotal += int64_t(writeBuffer.getSize());
				return true;
			}
			else
			{
				std::vector<char> buffer(10000);
				if (steamNetworkingSockets.GetDetailedConnectionStatus(steamNetConnection, buffer.data(), int(buffer.size())) == 0)
				{
					se::log::warning(se::formatString("Failed to send packet: %s", buffer.data()));
				}
				se_assert(!enableAssertOnSendFail && "Send failed");
				return false;
			}
		}

		void Connection2::disconnect(const std::string& reason)
		{
			disconnectImpl(reason, true);
			setStatus(Status::Disconnected);
		}

		void Connection2::disconnectImpl(const std::string& reason, const bool enableLinger)
		{
			if (steamNetConnection != k_HSteamNetConnection_Invalid)
			{
				closeConnectionImpl(steamNetConnection, reason, enableLinger);
				closedSteamNetConnection = steamNetConnection; // Used later on by the ConnectionManager
				steamNetConnection = k_HSteamNetConnection_Invalid;
			}
		}

		void Connection2::setStatus(const Status _status)
		{
			if (_status != status)
			{
				const Status oldStatus = status;
				status = _status;
				statusChangedSignal(oldStatus, status);
			}
		}

		void Connection2::receivePacket(const void* data, const size_t size, const bool reliable)
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
			if (receiveHandler && receivedPackets.empty())
			{
				ReadBuffer readBuffer(data, size);
				receiveHandler(readBuffer, reliable);
			}
			else
			{
				receivedPackets.push_back(ReceivedPacket());
				receivedPackets.back().reliable = reliable;
				receivedPackets.back().data.resize(size);
				memcpy(receivedPackets.back().data.data(), data, size);
			}
		}

		void Connection2::setSettings(const Settings& _settings)
		{
			setSettingsImpl(_settings, false);
		}

		void Connection2::setSettingsImpl(const Settings& _settings, const bool forceUpdate)
		{
			if (settings.sendBufferSize != _settings.sendBufferSize || forceUpdate)
			{
				se_assert(settings.sendBufferSize <= std::numeric_limits<uint32_t>::max());
				if (!SteamNetworkingUtils_Lib()->SetConnectionConfigValueInt32(steamNetConnection, k_ESteamNetworkingConfig_SendBufferSize, int32_t(_settings.sendBufferSize)))
				{
					se::log::error("Failed to set connection send buffer size.");
				}
			}
			if (settings.timeout != _settings.timeout || forceUpdate)
			{
				if (!SteamNetworkingUtils_Lib()->SetConnectionConfigValueInt32(steamNetConnection, k_ESteamNetworkingConfig_TimeoutConnected, int32_t(_settings.timeout.asMilliseconds())))
				{
					se::log::error("Failed to set connection timeout time.");
				}
			}
			settings = _settings;
		}

		time::Time Connection2::getPing() const
		{
			SteamNetConnectionRealTimeStatus_t steamNetConnectionRealTimeStatus;
			SteamNetConnectionRealTimeLaneStatus_t steamNetConnectionRealTimeLaneStatus;
			if (steamNetworkingSockets.GetConnectionRealTimeStatus(steamNetConnection, &steamNetConnectionRealTimeStatus, 0, &steamNetConnectionRealTimeLaneStatus))
			{
				return time::fromMilliseconds(float(steamNetConnectionRealTimeStatus.m_nPing));
			}
			else
			{
				return time::fromMilliseconds(9999.9f);
			}
		}

		uint16_t Connection2::getMaximumSegmentSize() const
		{
			return 1300; // internal value k_cbSteamNetworkingSocketsMaxUDPMsgLen
		}

		Connection2::DetailedStatus Connection2::getDetailedStatus() const
		{
			DetailedStatus detailedStatus;
			SteamNetConnectionRealTimeStatus_t steamNetConnectionRealTimeStatus;
			SteamNetConnectionRealTimeLaneStatus_t steamNetConnectionRealTimeLaneStatus;
			if (steamNetworkingSockets.GetConnectionRealTimeStatus(steamNetConnection, &steamNetConnectionRealTimeStatus, 0, &steamNetConnectionRealTimeLaneStatus))
			{
				detailedStatus.reliableBytesInSendQueue = steamNetConnectionRealTimeStatus.m_cbPendingReliable;
				detailedStatus.unreliableBytesInSendQueue = steamNetConnectionRealTimeStatus.m_cbPendingUnreliable;
				detailedStatus.sentUnackedReliableBytes = steamNetConnectionRealTimeStatus.m_cbSentUnackedReliable;
				detailedStatus.ping = time::fromMilliseconds(float(steamNetConnectionRealTimeStatus.m_nPing));
			}
			return detailedStatus;
		}

		void Connection2::closeConnectionImpl(const HSteamNetConnection _steamNetConnection, const std::string& reason, const bool enableLinger)
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
	}
}
