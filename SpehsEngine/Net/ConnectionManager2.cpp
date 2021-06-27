#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionManager2.h"

#include "SpehsEngine/Core/LockGuard.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Net/AddressUtilityFunctions.h"
#include <set>


#pragma optimize("", off)

namespace se
{
	namespace net
	{
		std::recursive_mutex ConnectionManager2::callbackConnectionManagerMutex;
		ConnectionManager2* ConnectionManager2::callbackConnectionManager = nullptr;

		ConnectionManager2::ConnectionManager2(const std::string_view _name)
			: name(_name)
		{
			steamNetworkingSockets = SteamNetworkingSockets();
			se_assert(steamNetworkingSockets);
			if (steamNetworkingSockets)
			{
				steamNetPollGroup = steamNetworkingSockets->CreatePollGroup();
				if (steamNetPollGroup == k_HSteamNetPollGroup_Invalid)
				{
					se::log::error("Failed to create steam net poll group.");
				}
			}
		}

		ConnectionManager2::~ConnectionManager2()
		{
			stopListening();
			for (std::shared_ptr<Connection2>& connection : connections)
			{
				connection->disconnect();
			}
			if (steamNetworkingSockets)
			{
				if (steamNetPollGroup != k_HSteamNetPollGroup_Invalid)
				{
					steamNetworkingSockets->DestroyPollGroup(steamNetPollGroup);
				}
			}
		}

		void ConnectionManager2::update()
		{
			if (steamNetworkingSockets)
			{
				// Run callbacks
				{
					std::lock_guard<std::recursive_mutex> lock(callbackConnectionManagerMutex);
					se_assert(!callbackConnectionManager);
					callbackConnectionManager = this;
					steamNetworkingSockets->RunCallbacks();
					callbackConnectionManager = nullptr;
				}

				if (steamNetPollGroup != k_HSteamNetPollGroup_Invalid)
				{
					ISteamNetworkingMessage* steamNetworkingMessage = nullptr;
					int messageCount = steamNetworkingSockets->ReceiveMessagesOnPollGroup(steamNetPollGroup, &steamNetworkingMessage, 1);
					if (messageCount < 0)
					{
						se::log::error("Error checking for messages");
					}
					else if (messageCount > 0)
					{
						assert(messageCount == 1 && steamNetworkingMessage);

						for (const std::shared_ptr<Connection2>& connection : connections)
						{
							if (connection->steamNetConnection == steamNetworkingMessage->m_conn)
							{
								const bool reliable = steamNetworkingMessage->m_nFlags == k_nSteamNetworkingSend_Reliable;
								connection->receivePacket(steamNetworkingMessage->m_pData, size_t(steamNetworkingMessage->m_cbSize), reliable);
								steamNetworkingMessage->Release();
							}
						}
					}
				}
			}

			for (size_t i = 0; i < connections.size(); i++)
			{
				connections[i]->update();
				if (connections[i].use_count() == 1)
				{
					connections[i]->disconnect("unreferenced connection");
				}
			}
			for (const std::shared_ptr<Connection2>& connection : connections)
			{
				connection->update();
			}
		}

		bool ConnectionManager2::startListening(const Port port)
		{
			stopListening();
			if (steamNetworkingSockets)
			{
				// Start listening
				SteamNetworkingIPAddr localSteamNetworkingAddress;
				localSteamNetworkingAddress.Clear();
				localSteamNetworkingAddress.m_port = port.value;
				SteamNetworkingConfigValue_t steamNetworkingConfigValue;
				steamNetworkingConfigValue.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallbackStatic);

				steamListenSocket = steamNetworkingSockets->CreateListenSocketIP(localSteamNetworkingAddress, 1, &steamNetworkingConfigValue);
				if (steamListenSocket == k_HSteamListenSocket_Invalid)
				{
					se::log::error("Failed to start listening on port: " + port.toString() + ". Failed to create listening socket.");
					return false;
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		void ConnectionManager2::stopListening()
		{
			if (steamListenSocket != k_HSteamListenSocket_Invalid)
			{
				steamNetworkingSockets->CloseListenSocket(steamListenSocket);
				steamListenSocket = k_HSteamListenSocket_Invalid;
			}
		}

		std::shared_ptr<Connection2> ConnectionManager2::connect(const se::net::Endpoint& endpoint)
		{
			if (steamNetworkingSockets)
			{
				SteamNetworkingIPAddr steamNetworkingAddress = toSteamNetworkingAddress(endpoint);
				SteamNetworkingConfigValue_t opt;
				opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallbackStatic);
				HSteamNetConnection steamNetConnection = steamNetworkingSockets->ConnectByIPAddress(steamNetworkingAddress, 1, &opt);
				if (steamNetConnection == k_HSteamNetConnection_Invalid)
				{
					se::log::error("Failed to connect: " + endpoint.toString());
					return std::shared_ptr<Connection2>();
				}
				else
				{
					std::shared_ptr<Connection2> connection(new Connection2(*steamNetworkingSockets, generateNewConnectionId(), endpoint, steamNetConnection, Connection2::EstablishmentType::Outgoing));
					connections.push_back(connection);
					return connection;
				}
			}
			else
			{
				return std::shared_ptr<Connection2>();
			}
		}

		void ConnectionManager2::steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t& info)
		{
			// For existing connections, let the connection handle the status change
			for (std::shared_ptr<Connection2>& connection : connections)
			{
				if (connection->steamNetConnection == info.m_hConn)
				{
					connection->steamNetConnectionStatusChangedCallback(info);
					return;
				}
			}

			switch (info.m_info.m_eState)
			{
			case k_ESteamNetworkingConnectionState_FindingRoute:
			case k_ESteamNetworkingConnectionState_FinWait:
			case k_ESteamNetworkingConnectionState_Linger:
			case k_ESteamNetworkingConnectionState__Force32Bit:
				break;
			case k_ESteamNetworkingConnectionState_Dead:
				se::log::error("Failed to accept connection: dead.");
				break;
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
				se::log::error("Failed to accept connection: problem detected locally.");
				break;
			case k_ESteamNetworkingConnectionState_None:
				// Called when a connection is being destroyed
				break;
			case k_ESteamNetworkingConnectionState_ClosedByPeer:
				se::log::error("Failed to accept connection: connection closed by peer.");
				break;
			case k_ESteamNetworkingConnectionState_Connected:
				se_assert(false && "Unexpected");
				break;
			case k_ESteamNetworkingConnectionState_Connecting:
			{
				// This must be a new connection
				se_assert(steamNetworkingSockets);

				// A client is attempting to connect
				// Try to accept the connection.
				HSteamNetConnection steamNetConnection = info.m_hConn;
				const EResult acceptConnectionResult = steamNetworkingSockets->AcceptConnection(steamNetConnection);
				if (acceptConnectionResult != k_EResultOK)
				{
					// ...Is local connection?
					if (acceptConnectionResult == k_EResultInvalidParam)
					{
						SteamNetworkingConfigValue_t opt;
						opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallbackStatic);
						steamNetConnection = steamNetworkingSockets->ConnectByIPAddress(info.m_info.m_addrRemote, 1, &opt);
						if (steamNetConnection == k_HSteamNetConnection_Invalid)
						{
							se::log::error("Failed to connect: " + std::string(info.m_info.m_szConnectionDescription));
							break;
						}
						else
						{
							if (!steamNetworkingSockets->CreateSocketPair(&info.m_hConn, &steamNetConnection, true, nullptr, nullptr))
							{
								se::log::error("Failed to connect: failed to create socket pair.");
							}
						}
					}
					else
					{
						steamNetworkingSockets->CloseConnection(steamNetConnection, 0, nullptr, false);
						se::log::warning("Failed to accept incoming connection from: " + std::string(info.m_info.m_szConnectionDescription) + ". Error code: " + std::to_string(acceptConnectionResult));
						break;
					}
				}

				// Assign the poll group
				if (steamNetPollGroup != k_HSteamNetPollGroup_Invalid)
				{
					if (!steamNetworkingSockets->SetConnectionPollGroup(steamNetConnection, steamNetPollGroup))
					{
						steamNetworkingSockets->CloseConnection(steamNetConnection, 0, nullptr, false);
						se::log::warning("Failed to accept incoming connection from: " + std::string(info.m_info.m_szConnectionDescription) + ". Failed to set the steam net poll group.");
						break;
					}
				}

				se::log::info("Accepted new connection: " + std::string(info.m_info.m_szConnectionDescription));

				std::shared_ptr<Connection2> newConnection(new Connection2(*steamNetworkingSockets, generateNewConnectionId(),
					fromSteamNetworkingAddress(info.m_info.m_addrRemote), steamNetConnection, Connection2::EstablishmentType::Incoming));
				connections.push_back(newConnection);

				incomingConnectionSignal(newConnection);
				break;
			}
			}
		}

		void ConnectionManager2::steamNetConnectionStatusChangedCallbackStatic(SteamNetConnectionStatusChangedCallback_t* info)
		{
			std::lock_guard<std::recursive_mutex> lock(callbackConnectionManagerMutex);
			se_assert(callbackConnectionManager);
			if (callbackConnectionManager)
			{
				callbackConnectionManager->steamNetConnectionStatusChangedCallback(*info);
			}
		}

		ConnectionId ConnectionManager2::generateNewConnectionId()
		{
			// Keep a list of randomly generated connection ids during this program run to minimize clashes
			static std::set<ConnectionId::ValueType> usedConnectionIds;
			static std::mutex usedConnectionIdsMutex;

			ConnectionId connectionId;
			while (!connectionId || usedConnectionIds.find(connectionId.value) != usedConnectionIds.end())
			{
				connectionId.value = rng::random<ConnectionId::ValueType>();
			}

			usedConnectionIds.insert(connectionId.value);
			return connectionId;
		}
	}
}
