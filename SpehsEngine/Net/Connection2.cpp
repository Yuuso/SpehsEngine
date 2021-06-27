#include "stdafx.h"
#include "SpehsEngine/Net/Connection2.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/PrecompiledInclude.h"


#pragma optimize("", off)

namespace se
{
	namespace net
	{
		Connection2::Connection2(ISteamNetworkingSockets& _steamNetworkingSockets, const ConnectionId _connectionId, const se::net::Endpoint& _remoteEndpoint,
			const HSteamNetConnection _steamNetConnection, const EstablishmentType _establishmentType)
			: establishmentType(_establishmentType)
			, connectionId(_connectionId)
			, remoteEndpoint(_remoteEndpoint)
			, steamNetworkingSockets(_steamNetworkingSockets)
			, steamNetConnection(_steamNetConnection)
		{
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
			const int flags = reliable ? 0 : k_nSteamNetworkingSend_Unreliable;
			const EResult result = steamNetworkingSockets.SendMessageToConnection(steamNetConnection, writeBuffer.getData(), uint32_t(writeBuffer.getSize()), flags, nullptr);
			if (result == k_EVoiceResultOK)
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
				return false;
			}
		}

		void Connection2::disconnect(const std::string& reason)
		{
			disconnectImpl(reason, true);
		}

		void Connection2::disconnectImpl(const std::string& reason, const bool enableLinger)
		{
			if (steamNetConnection != k_HSteamNetConnection_Invalid)
			{
				steamNetworkingSockets.CloseConnection(steamNetConnection, 0, reason.empty() ? "Connection::disconnect()" : reason.c_str(), true);
				steamNetConnection = k_HSteamNetConnection_Invalid;
			}
		}

		void Connection2::steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t& info)
		{
			switch (info.m_info.m_eState)
			{
			case k_ESteamNetworkingConnectionState_FindingRoute:
			case k_ESteamNetworkingConnectionState_Dead:
			case k_ESteamNetworkingConnectionState_FinWait:
			case k_ESteamNetworkingConnectionState_Linger:
			case k_ESteamNetworkingConnectionState__Force32Bit:
				break;
			case k_ESteamNetworkingConnectionState_None:
				// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
				break;

			case k_ESteamNetworkingConnectionState_ClosedByPeer:
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			{
				const char* pszDebugLogAction;
				if (info.m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
				{
					pszDebugLogAction = "problem detected locally";
				}
				else
				{
					// Note that here we could check the reason code to see if it was a "usual" connection or an "unusual" one.
					pszDebugLogAction = "closed by peer";
				}

				se::log::info(se::formatString("Connection %s %s, reason %d: %s\n",
					info.m_info.m_szConnectionDescription,
					pszDebugLogAction,
					info.m_info.m_eEndReason,
					info.m_info.m_szEndDebug));

				// Clean up the connection. This is important!
				// The connection is "closed" in the network sense, but
				// it has not been destroyed.  We must close it on our end, too
				// to finish up.  The reason information do not matter in this case,
				// and we cannot linger because it's already closed on the other end,
				// so we just pass 0's.
				disconnectImpl("ProblemDetectedLocally", false);

				setStatus(Status::Disconnected);
				break;
			}

			case k_ESteamNetworkingConnectionState_Connecting:
				se_assert(getStatus() == Status::Connecting);
				break;

			case k_ESteamNetworkingConnectionState_Connected:
				setStatus(Status::Connected);
				break;
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
	}
}
