#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionManager2.h"

#include "SpehsEngine/Core/LockGuard.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Net/AddressUtilityFunctions.h"
#include "steam/isteamnetworkingutils.h"
#include <atomic>
#include <set>


namespace se
{
	namespace net
	{
		namespace
		{
			std::optional<HSteamListenSocket> getSteamListenSocket(ISteamNetworkingSockets& steamNetworkingSockets, const HSteamNetConnection steamNetConnection)
			{
				SteamNetConnectionInfo_t steamNetConnectionInfo;
				if (steamNetworkingSockets.GetConnectionInfo(steamNetConnection, &steamNetConnectionInfo))
				{
					return steamNetConnectionInfo.m_hListenSocket;
				}
				return std::nullopt;
			}

			std::optional<Port> getLocalListeningPort(ISteamNetworkingSockets& steamNetworkingSockets, const HSteamNetConnection steamNetConnection)
			{
				if (std::optional<HSteamListenSocket> steamListenSocket = getSteamListenSocket(steamNetworkingSockets, steamNetConnection))
				{
					SteamNetworkingIPAddr steamNetworkingAddress;
					while (!steamNetworkingSockets.GetListenSocketAddress(*steamListenSocket, &steamNetworkingAddress))
					{
						return Port(steamNetworkingAddress.m_port);
					}
				}
				return std::nullopt;
			}

			std::mutex connectionManagersMutex;
			std::vector<ConnectionManager2*> connectionManagers;
		}

		void ConnectionManager2::steamNetConnectionStatusChangedCallbackStatic(SteamNetConnectionStatusChangedCallback_t* const info)
		{
			switch (info->m_info.m_eState)
			{
			case k_ESteamNetworkingConnectionState_FindingRoute:
			case k_ESteamNetworkingConnectionState_FinWait:
			case k_ESteamNetworkingConnectionState_Linger:
			case k_ESteamNetworkingConnectionState__Force32Bit:
				break;
			case k_ESteamNetworkingConnectionState_None:
				// Called when a connection is being destroyed
				break;
			case k_ESteamNetworkingConnectionState_Dead:
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			case k_ESteamNetworkingConnectionState_ClosedByPeer:
			{
				std::lock_guard<std::mutex> lock(connectionManagersMutex);
				for (ConnectionManager2* const connectionManager : connectionManagers)
				{
					if (connectionManager->ownsConnection(info->m_hConn))
					{
						std::lock_guard<std::recursive_mutex> lock2(connectionManager->connectionStatusChangeQueueMutex);
						connectionManager->connectionStatusChangeQueue.push_back(ConnectionStatusChange());
						connectionManager->connectionStatusChangeQueue.back().steamNetConnection = info->m_hConn;
						connectionManager->connectionStatusChangeQueue.back().status = Connection2::Status::Disconnected;
						if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_Dead)
						{
							connectionManager->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_Dead";
						}
						else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
						{
							connectionManager->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_ProblemDetectedLocally";
						}
						else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer)
						{
							connectionManager->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_ClosedByPeer";
						}
						else
						{
							se::log::error("unhandled k_ESteamNetworkingConnectionState");
						}
						return;
					}
				}
				log::error("No connection manager owns the connection: " + std::string(info->m_info.m_szConnectionDescription));
				return;
			}
			case k_ESteamNetworkingConnectionState_Connecting:
			{
				// HACK: how to check for application local connection?
				if (doesContain(info->m_info.m_szConnectionDescription, "invalid"))
				{
					return;
				}

				// Find an accepting connection manager
				const std::optional<HSteamListenSocket> steamListenSocket = getSteamListenSocket(*SteamNetworkingSockets(), info->m_hConn);
				if (!steamListenSocket)
				{
					Connection2::closeConnectionImpl(info->m_hConn, "", false);
					se::log::error("Failed to accept incoming connection from: " + std::string(info->m_info.m_szConnectionDescription) + ". Could not get steam listen socket.");
					return;
				}

				std::lock_guard<std::mutex> lock(connectionManagersMutex);
				ConnectionManager2* connectionManager = nullptr;
				for (ConnectionManager2* const connectionManager2 : connectionManagers)
				{
					std::lock_guard<std::recursive_mutex> lock2(connectionManager2->acceptingSteamListenSocketMutex);
					if (connectionManager2->acceptingSteamListenSocket == steamListenSocket)
					{
						connectionManager = connectionManager2;
						break;
					}
				}
				if (!connectionManager)
				{
					log::error("No connection manager is currently accepting a new connection: " + std::string(info->m_info.m_szConnectionDescription));
					return;
				}

				// A client is attempting to connect
				// Try to accept the connection.
				const EResult acceptConnectionResult = SteamNetworkingSockets()->AcceptConnection(info->m_hConn);
				if (acceptConnectionResult != k_EResultOK)
				{
					Connection2::closeConnectionImpl(info->m_hConn, "", false);
					se::log::error("Failed to accept incoming connection from: " + std::string(info->m_info.m_szConnectionDescription) + ". Error code: " + std::to_string(acceptConnectionResult));
					return;
				}

				se::log::info("Accepted new connection: " + std::string(info->m_info.m_szConnectionDescription));
				Connection2::ConstructorParameters connectionConstructorParameters;
				connectionConstructorParameters.status = Connection2::Status::Connected;
				connectionConstructorParameters.establishmentType = Connection2::EstablishmentType::Incoming;
				connectionConstructorParameters.remoteEndpoint = fromSteamNetworkingAddress(info->m_info.m_addrRemote);
				connectionConstructorParameters.steamNetConnection = info->m_hConn;
				connectionConstructorParameters.steamNetworkingSockets = SteamNetworkingSockets();
				connectionConstructorParameters.steamListenSocket = *steamListenSocket;

				{
					std::lock_guard<std::recursive_mutex> lock2(connectionManager->ownedSteamNetConnectionsMutex);
					connectionManager->ownedSteamNetConnections.insert(info->m_hConn);
				}
				{
					std::lock_guard<std::recursive_mutex> lock2(connectionManager->incomingConnectionQueueMutex);
					connectionManager->incomingConnectionQueue.push_back(connectionConstructorParameters);
				}
				return;
			}
			case k_ESteamNetworkingConnectionState_Connected:
				std::lock_guard<std::mutex> lock(connectionManagersMutex);
				for (ConnectionManager2* const connectionManager : connectionManagers)
				{
					if (connectionManager->ownsConnection(info->m_hConn))
					{
						std::lock_guard<std::recursive_mutex> lock2(connectionManager->connectionStatusChangeQueueMutex);
						connectionManager->connectionStatusChangeQueue.push_back(ConnectionStatusChange());
						connectionManager->connectionStatusChangeQueue.back().steamNetConnection = info->m_hConn;
						connectionManager->connectionStatusChangeQueue.back().status = Connection2::Status::Connected;
						connectionManager->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_Connected";
						return;
					}
				}
				log::error("No connection manager owns the connection: " + std::string(info->m_info.m_szConnectionDescription));
				return;
			}
		}

		// ...

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

			std::lock_guard<std::mutex> lock(connectionManagersMutex);
			connectionManagers.push_back(this);
		}

		ConnectionManager2::~ConnectionManager2()
		{
			stopAccepting();
			processIncomingConnectionQueue();
			processConnectionStatusChangeQueue();
			closeUnusedSteamListenSockets();
			for (std::shared_ptr<Connection2>& connection : connections)
			{
				connection->disconnect();
			}
			closeUnusedSteamListenSockets();
			if (steamNetworkingSockets)
			{
				if (steamNetPollGroup != k_HSteamNetPollGroup_Invalid)
				{
					steamNetworkingSockets->DestroyPollGroup(steamNetPollGroup);
				}
			}

			std::lock_guard<std::mutex> lock(connectionManagersMutex);
			for (size_t i = 0; i < connectionManagers.size(); i++)
			{
				if (connectionManagers[i] == this)
				{
					std::swap(connectionManagers[i], connectionManagers.back());
					connectionManagers.pop_back();
				}
			}
		}

		void ConnectionManager2::update()
		{
			if (steamNetworkingSockets)
			{
				// Run GLOBAL callbacks
				// If multiple connection managers exist, this gets called more often but that probably doesn't matter.
				steamNetworkingSockets->RunCallbacks();

				processIncomingConnectionQueue();
				processConnectionStatusChangeQueue();

				const time::Time beginTime = time::now();
				const time::Time maxTime = time::fromSeconds(1.0f / 120.0f);
				while (steamNetPollGroup != k_HSteamNetPollGroup_Invalid && (time::now() - beginTime <= maxTime))
				{
					ISteamNetworkingMessage* steamNetworkingMessages[16];
					int messageCount = steamNetworkingSockets->ReceiveMessagesOnPollGroup(steamNetPollGroup, steamNetworkingMessages, 16);
					if (messageCount == 0)
					{
						break;
					}
					else if (messageCount < 0)
					{
						se::log::error("Error checking for messages");
					}
					else
					{
						for (int m = 0; m < messageCount; m++)
						{
							se_assert(steamNetworkingMessages[m]);
							for (const std::shared_ptr<Connection2>& connection : connections)
							{
								if (connection->steamNetConnection == steamNetworkingMessages[m]->m_conn)
								{
									const bool reliable = steamNetworkingMessages[m]->m_nFlags == k_nSteamNetworkingSend_Reliable;
									connection->receivePacket(steamNetworkingMessages[m]->m_pData, size_t(steamNetworkingMessages[m]->m_cbSize), reliable);
								}
							}
							steamNetworkingMessages[m]->Release();
						}
					}
				}
			}

			for (size_t i = 0; i < connections.size(); i++)
			{
				connections[i]->update();
				if (removeUnreferencedConnections && connections[i].use_count() == 1)
				{
					connections[i]->disconnect("unreferenced connection");
				}

				if (connections[i]->isDisconnected())
				{
					if (connections[i]->closedSteamNetConnection != k_HSteamNetConnection_Invalid)
					{
						std::lock_guard<std::recursive_mutex> lock(ownedSteamNetConnectionsMutex);
						const std::unordered_set<HSteamNetConnection>::iterator it = ownedSteamNetConnections.find(connections[i]->closedSteamNetConnection);
						se_assert(it != ownedSteamNetConnections.end());
						ownedSteamNetConnections.erase(it);
						connections[i]->closedSteamNetConnection = k_HSteamNetConnection_Invalid;
					}

					if (connections[i]->getStatus() == Connection2::Status::Disconnected)
					{
						connections[i] = connections.back();
						connections.pop_back();
						i--;
						closeUnusedSteamListenSockets();
					}
				}
			}
			for (const std::shared_ptr<Connection2>& connection : connections)
			{
				connection->update();
			}
		}

		void ConnectionManager2::processIncomingConnectionQueue()
		{
			std::vector<Connection2::ConstructorParameters> temp;
			{
				std::lock_guard<std::recursive_mutex> lock(incomingConnectionQueueMutex);
				std::swap(temp, incomingConnectionQueue);
			}
			for (const Connection2::ConstructorParameters& constructorParameters : temp)
			{
				{
					std::lock_guard<std::recursive_mutex> lock(acceptingSteamListenSocketMutex);
					if (acceptingSteamListenSocket == k_HSteamListenSocket_Invalid)
					{
						break;
					}
				}
				std::shared_ptr<Connection2> connection(new Connection2(constructorParameters));
				initializeSteamNetConnection(*connection);
				connections.push_back(connection);
				incomingConnectionSignal(connection);
			}
		}

		void ConnectionManager2::processConnectionStatusChangeQueue()
		{
			std::vector<ConnectionStatusChange> temp;
			{
				std::lock_guard<std::recursive_mutex> lock(connectionStatusChangeQueueMutex);
				std::swap(temp, connectionStatusChangeQueue);
			}
			for (const ConnectionStatusChange& connectionStatusChange : temp)
			{
				for (size_t i = 0; i < connections.size(); i++)
				{
					if (connections[i]->steamNetConnection == connectionStatusChange.steamNetConnection)
					{
						connections[i]->setStatus(connectionStatusChange.status);
						switch (connectionStatusChange.status)
						{
						case Connection2::Status::Connecting:
							break;
						case Connection2::Status::Connected:
							break;
						case Connection2::Status::Disconnected:
							connections[i]->disconnectImpl(connectionStatusChange.reason, false);
							break;
						}
					}
				}
			}
		}

		void ConnectionManager2::closeUnusedSteamListenSockets()
		{
			for (size_t i = 0; i < steamListenSockets.size(); i++)
			{
				bool remove = true;
				for (const std::shared_ptr<Connection2>& connection : connections)
				{
					if (connection->steamListenSocket == steamListenSockets[i] && !connection->isDisconnected())
					{
						remove = false;
						break;
					}
				}
				if (remove)
				{
					steamNetworkingSockets->CloseListenSocket(steamListenSockets[i]);
					steamListenSockets[i] = steamListenSockets.back();
					steamListenSockets.pop_back();
				}
			}
		}

		bool ConnectionManager2::ownsConnection(const HSteamNetConnection steamNetConnection) const
		{
			std::lock_guard<std::recursive_mutex> lock(ownedSteamNetConnectionsMutex);
			return ownedSteamNetConnections.find(steamNetConnection) != ownedSteamNetConnections.end();
		}

		bool ConnectionManager2::startAccepting(const std::optional<Port> port)
		{
			std::lock_guard<std::recursive_mutex> lock(acceptingSteamListenSocketMutex);
			if (acceptingSteamListenSocket != k_HSteamListenSocket_Invalid)
			{
				if (const std::optional<Port> existingAcceptingPort = getAcceptingPort())
				{
					se::log::error("Already accepting on port: " + existingAcceptingPort->toString());
				}
				return false;
			}

			if (steamNetworkingSockets)
			{
				// Check if we already have a listening socket for this port
				if (port)
				{
					SteamNetworkingIPAddr steamNetworkingAddress;
					for (size_t i = 0; i < steamListenSockets.size(); i++)
					{
						if (steamNetworkingSockets->GetListenSocketAddress(steamListenSockets[i], &steamNetworkingAddress))
						{
							if (port->value == steamNetworkingAddress.m_port)
							{
								acceptingSteamListenSocket = steamListenSockets[i];
								steamListenSockets[i] = steamListenSockets.back();
								steamListenSockets.pop_back();
								return true;
							}
						}
					}
				}

				// Start listening from a new port
				SteamNetworkingIPAddr localSteamNetworkingAddress;
				localSteamNetworkingAddress.Clear();
				if (port)
				{
					localSteamNetworkingAddress.m_port = port->value;
				}

				std::vector<SteamNetworkingConfigValue_t> steamNetworkingConfigValues;
				steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
				steamNetworkingConfigValues.back().SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallbackStatic);
				HSteamListenSocket steamListenSocket = steamNetworkingSockets->CreateListenSocketIP(localSteamNetworkingAddress, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
				if (steamListenSocket != k_HSteamListenSocket_Invalid)
				{
					acceptingSteamListenSocket = steamListenSocket;
					return true;
				}
				else
				{
					se::log::error("Failed to start listening on port: " + std::to_string(localSteamNetworkingAddress.m_port) + ". Failed to create listening socket.");
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		void ConnectionManager2::stopAccepting()
		{
			std::lock_guard<std::recursive_mutex> lock(acceptingSteamListenSocketMutex);
			if (acceptingSteamListenSocket != k_HSteamListenSocket_Invalid)
			{
				steamListenSockets.push_back(acceptingSteamListenSocket);
				acceptingSteamListenSocket = k_HSteamListenSocket_Invalid;
				closeUnusedSteamListenSockets();
			}
		}

		std::optional<Port> ConnectionManager2::getAcceptingPort() const
		{
			std::lock_guard<std::recursive_mutex> lock(acceptingSteamListenSocketMutex);
			if (acceptingSteamListenSocket != k_HSteamListenSocket_Invalid)
			{
				SteamNetworkingIPAddr steamNetworkingAddress;
				if (steamNetworkingSockets->GetListenSocketAddress(acceptingSteamListenSocket, &steamNetworkingAddress))
				{
					return Port(steamNetworkingAddress.m_port);
				}
			}
			return std::nullopt;
		}

		void ConnectionManager2::initializeSteamNetConnection(Connection2& connection)
		{
			if (!steamNetworkingSockets->SetConnectionPollGroup(connection.steamNetConnection, steamNetPollGroup))
			{
				se::log::error("Failed to set connection poll group.");
			}

			std::lock_guard<std::recursive_mutex> lock(ownedSteamNetConnectionsMutex);
			ownedSteamNetConnections.insert(connection.steamNetConnection);
		}

		std::shared_ptr<Connection2> ConnectionManager2::connect(const se::net::Endpoint& _endpoint, const bool _symmetric, const std::string_view _name, const time::Time _timeout)
		{
			if (steamNetworkingSockets)
			{
				std::vector<SteamNetworkingConfigValue_t> steamNetworkingConfigValues;
				steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
				steamNetworkingConfigValues.back().SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallbackStatic);
				steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
				steamNetworkingConfigValues.back().SetInt32(k_ESteamNetworkingConfig_TimeoutInitial, int(_timeout.asMilliseconds()));
				if (_symmetric)
				{
					steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
					steamNetworkingConfigValues.back().SetInt32(k_ESteamNetworkingConfig_SymmetricConnect, 1);
				}
				const SteamNetworkingIPAddr steamNetworkingAddress = toSteamNetworkingAddress(_endpoint);
				HSteamNetConnection steamNetConnection = k_HSteamNetConnection_Invalid;

				{
					std::lock_guard<std::recursive_mutex> lock(ownedSteamNetConnectionsMutex);
					steamNetConnection = steamNetworkingSockets->ConnectByIPAddress(steamNetworkingAddress, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
					if (steamNetConnection != k_HSteamNetConnection_Invalid)
					{
						ownedSteamNetConnections.insert(steamNetConnection);
					}
				}

				if (steamNetConnection == k_HSteamNetConnection_Invalid)
				{
					se::log::error("Failed to connect: " + _endpoint.toString());
					return std::shared_ptr<Connection2>();
				}
				else
				{
					Connection2::ConstructorParameters connectionConstructorParameters;
					connectionConstructorParameters.establishmentType = Connection2::EstablishmentType::Outgoing;
					connectionConstructorParameters.name = _name;
					connectionConstructorParameters.remoteEndpoint = _endpoint;
					connectionConstructorParameters.steamNetConnection = steamNetConnection;
					connectionConstructorParameters.steamNetworkingSockets = steamNetworkingSockets;
					connectionConstructorParameters.localListeningPort = getLocalListeningPort(*steamNetworkingSockets, steamNetConnection);
					std::shared_ptr<Connection2> connection(new Connection2(connectionConstructorParameters));
					initializeSteamNetConnection(*connection);
					connections.push_back(connection);
					return connection;
				}
			}
			else
			{
				return std::shared_ptr<Connection2>();
			}
		}

		static std::recursive_mutex connectionSimulationSettingsMutex;
		static ConnectionSimulationSettings connectionSimulationSettings;
		ConnectionSimulationSettings ConnectionManager2::getConnectionSimulationSettings()
		{
			std::lock_guard<std::recursive_mutex> lock(connectionSimulationSettingsMutex);
			return connectionSimulationSettings;
		}
		void ConnectionManager2::setConnectionSimulationSettings(const ConnectionSimulationSettings& _connectionSimulationSettings)
		{
			std::lock_guard<std::recursive_mutex> lock(connectionSimulationSettingsMutex);
			if (connectionSimulationSettings.chanceToDropIncoming != _connectionSimulationSettings.chanceToDropOutgoing)
			{
				SteamNetworkingUtils_Lib()->SetGlobalConfigValueFloat(k_ESteamNetworkingConfig_FakePacketLoss_Send, _connectionSimulationSettings.chanceToDropOutgoing * 100.0f);
			}
			if (connectionSimulationSettings.chanceToDropIncoming != _connectionSimulationSettings.chanceToDropIncoming)
			{
				SteamNetworkingUtils_Lib()->SetGlobalConfigValueFloat(k_ESteamNetworkingConfig_FakePacketLoss_Recv, _connectionSimulationSettings.chanceToDropIncoming * 100.0f);
			}
			if (connectionSimulationSettings.chanceToReorderOutgoing != _connectionSimulationSettings.chanceToReorderOutgoing)
			{
				SteamNetworkingUtils_Lib()->SetGlobalConfigValueFloat(k_ESteamNetworkingConfig_FakePacketReorder_Send, _connectionSimulationSettings.chanceToReorderOutgoing * 100.0f);
			}
			if (connectionSimulationSettings.chanceToReorderIncoming != _connectionSimulationSettings.chanceToReorderIncoming)
			{
				SteamNetworkingUtils_Lib()->SetGlobalConfigValueFloat(k_ESteamNetworkingConfig_FakePacketReorder_Recv, _connectionSimulationSettings.chanceToReorderIncoming * 100.0f);
			}
			if (connectionSimulationSettings.delayOutgoing != _connectionSimulationSettings.delayOutgoing)
			{
				SteamNetworkingUtils_Lib()->SetGlobalConfigValueInt32(k_ESteamNetworkingConfig_FakePacketLag_Send, int(_connectionSimulationSettings.delayOutgoing.asMilliseconds()));
			}
			if (connectionSimulationSettings.delayIncoming != _connectionSimulationSettings.delayIncoming)
			{
				SteamNetworkingUtils_Lib()->SetGlobalConfigValueInt32(k_ESteamNetworkingConfig_FakePacketLag_Recv, int(_connectionSimulationSettings.delayIncoming.asMilliseconds()));
			}

			connectionSimulationSettings = _connectionSimulationSettings;
		}
	}
}
