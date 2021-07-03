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
#include "steam/isteamnetworkingutils.h"
#include <atomic>
#include <set>


#pragma optimize("", off)

namespace se
{
	namespace net
	{
		namespace
		{
			std::recursive_mutex callbackConnectionManagerMutex;
			ConnectionManager2* callbackConnectionManager = nullptr;

			// Super-hack for supporting multiple connection managers from the same application...
			// GameNetworkingSockets doesn't support connecting and accepting connections from within the same application.
			std::mutex connectionManagersMutex;
			std::vector<ConnectionManager2*> connectionManagers;
		}

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
			processLocalRemoteConnectionQueue();
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
			processLocalRemoteConnectionQueue();

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

		void ConnectionManager2::processLocalRemoteConnectionQueue()
		{
			std::lock_guard<std::mutex> lock(localRemoteConnectionQueueMutex);
			for (const LocalRemoteConnection& localRemoteConnection : localRemoteConnectionQueue)
			{
				for (std::shared_ptr<Connection2>& connection : connections)
				{
					if (connection->steamNetConnection == localRemoteConnection.fromSteamNetConnection)
					{
						Connection2::closeConnectionImpl(connection->steamNetConnection, "processLocalRemoteConnectionQueue()", false);
						{
							std::lock_guard<std::recursive_mutex> lock2(ownedSteamNetConnectionsMutex);
							const std::unordered_set<HSteamNetConnection>::iterator it = ownedSteamNetConnections.find(localRemoteConnection.fromSteamNetConnection);
							se_assert(it != ownedSteamNetConnections.end());
							ownedSteamNetConnections.erase(it);
							ownedSteamNetConnections.insert(localRemoteConnection.toSteamNetConnection);
						}
						SteamNetworkingUtils()->SetConfigValue(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, k_ESteamNetworkingConfig_Connection,
							(intptr_t)&localRemoteConnection.toSteamNetConnection, k_ESteamNetworkingConfig_Ptr, (void*)steamNetConnectionStatusChangedCallbackStatic);
						connection->steamNetConnection = localRemoteConnection.toSteamNetConnection;
						connection->localRemoteSteamNetConnection = localRemoteConnection.localRemoteSteamNetConnection;
						connection->localConnectionLifeline = localRemoteConnection.localConnectionLifeline;
						se_assert(connection->status != Connection2::Status::Connected);
						connection->setStatus(Connection2::Status::Connected);
						initializeSteamNetConnection(*connection);
						se::log::info("Local connection established (HACK)");
						break;
					}
				}
			}
			localRemoteConnectionQueue.clear();
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

		bool ConnectionManager2::startAccepting(const std::optional<Port> port)
		{
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
				acceptingSteamListenSocket = steamNetworkingSockets->CreateListenSocketIP(localSteamNetworkingAddress, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
				if (acceptingSteamListenSocket == k_HSteamListenSocket_Invalid)
				{
					se::log::error("Failed to start listening on port: " + std::to_string(localSteamNetworkingAddress.m_port) + ". Failed to create listening socket.");
					return false;
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		void ConnectionManager2::stopAccepting()
		{
			if (acceptingSteamListenSocket != k_HSteamListenSocket_Invalid)
			{
				steamListenSockets.push_back(acceptingSteamListenSocket);
				acceptingSteamListenSocket = k_HSteamListenSocket_Invalid;
				closeUnusedSteamListenSockets();
			}
		}

		std::optional<Port> ConnectionManager2::getAcceptingPort() const
		{
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
				// Acquire this mutex here so that another local ConnectionManager cannot grab it before we get to set the owned connection
				std::lock_guard<std::recursive_mutex> lock(ownedSteamNetConnectionsMutex);

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
				const HSteamNetConnection steamNetConnection = steamNetworkingSockets->ConnectByIPAddress(steamNetworkingAddress, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
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
			case k_ESteamNetworkingConnectionState_None:
				// Called when a connection is being destroyed
				break;
			case k_ESteamNetworkingConnectionState_Connected:
				se_assert(false && "Unexpected");
				break;
			case k_ESteamNetworkingConnectionState_Dead:
				se::log::error("Failed to accept connection: dead.");
				break;
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
				se::log::error("Failed to accept connection: problem detected locally.");
				break;
			case k_ESteamNetworkingConnectionState_ClosedByPeer:
				se::log::error("Failed to accept connection: connection closed by peer.");
				break;
			case k_ESteamNetworkingConnectionState_Connecting:
			{
				// This must be a new connection
				se_assert(steamNetworkingSockets);

				// A client is attempting to connect
				// Try to accept the connection.
				const EResult acceptConnectionResult = steamNetworkingSockets->AcceptConnection(info.m_hConn);
				if (acceptConnectionResult != k_EResultOK)
				{
					// ...Is local connection?
					if (acceptConnectionResult == k_EResultInvalidParam)
					{
						HSteamNetConnection steamNetConnection1 = k_HSteamNetConnection_Invalid;
						HSteamNetConnection steamNetConnection2 = k_HSteamNetConnection_Invalid;
						if (steamNetworkingSockets->CreateSocketPair(&steamNetConnection1, &steamNetConnection2, true, nullptr, nullptr))
						{
							se_assert(steamNetConnection1 != k_HSteamNetConnection_Invalid);
							se_assert(steamNetConnection2 != k_HSteamNetConnection_Invalid);
							
							//SteamNetworkingConfigValue_t steamNetworkingConfigValue;
							//steamNetworkingConfigValue.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallbackStatic);
							// TODO: how to set callback for connections created using CreateSocketPair()?
							SteamNetworkingUtils()->SetConfigValue(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, k_ESteamNetworkingConfig_Connection,
								(intptr_t)&steamNetConnection1, k_ESteamNetworkingConfig_Ptr, (void*)steamNetConnectionStatusChangedCallbackStatic);

							std::shared_ptr<bool> localConnectionLifeline(new bool(true));
							{
								std::lock_guard<std::mutex> lock(connectionManagersMutex);
								bool found = false;
								for (ConnectionManager2 *const connectionManager : connectionManagers)
								{
									std::lock_guard<std::recursive_mutex> lock2(connectionManager->ownedSteamNetConnectionsMutex);
									if (connectionManager->ownedSteamNetConnections.find(info.m_hConn) !=
										connectionManager->ownedSteamNetConnections.end())
									{
										std::lock_guard<std::mutex> lock3(connectionManager->localRemoteConnectionQueueMutex);
										connectionManager->localRemoteConnectionQueue.push_back(LocalRemoteConnection());
										connectionManager->localRemoteConnectionQueue.back().fromSteamNetConnection = info.m_hConn;
										connectionManager->localRemoteConnectionQueue.back().toSteamNetConnection = steamNetConnection2;
										connectionManager->localRemoteConnectionQueue.back().localRemoteSteamNetConnection = steamNetConnection1;
										connectionManager->localRemoteConnectionQueue.back().localConnectionLifeline = localConnectionLifeline;
										found = true;
										break;
									}
								}
								se_assert(found);
							}

							Connection2::ConstructorParameters connectionConstructorParameters;
							connectionConstructorParameters.establishmentType = Connection2::EstablishmentType::Incoming;
							connectionConstructorParameters.remoteEndpoint = fromSteamNetworkingAddress(info.m_info.m_addrRemote);
							connectionConstructorParameters.steamNetConnection = steamNetConnection1;
							connectionConstructorParameters.steamNetworkingSockets = steamNetworkingSockets;
							std::shared_ptr<Connection2> connection(new Connection2(connectionConstructorParameters));
							initializeSteamNetConnection(*connection);
							connection->status = Connection2::Status::Connected;
							connection->localRemoteSteamNetConnection = steamNetConnection2;
							connection->localConnectionLifeline = localConnectionLifeline;
							connections.push_back(connection);
							incomingConnectionSignal(connection);
							return;
						}
						else
						{
							se::log::error("Failed to connect: failed to create socket pair.");
						}
					}
					else
					{
						Connection2::closeConnectionImpl(info.m_hConn, "", false);
						se::log::warning("Failed to accept incoming connection from: " + std::string(info.m_info.m_szConnectionDescription) + ". Error code: " + std::to_string(acceptConnectionResult));
						break;
					}
				}

				// Assign the poll group
				if (!steamNetworkingSockets->SetConnectionPollGroup(info.m_hConn, steamNetPollGroup))
				{
					Connection2::closeConnectionImpl(info.m_hConn, "", false);
					se::log::error("Failed to accept incoming connection from: " + std::string(info.m_info.m_szConnectionDescription) + ". Failed to set the steam net poll group.");
				}

				se::log::info("Accepted new connection: " + std::string(info.m_info.m_szConnectionDescription));

				Connection2::ConstructorParameters connectionConstructorParameters;
				connectionConstructorParameters.establishmentType = Connection2::EstablishmentType::Incoming;
				connectionConstructorParameters.remoteEndpoint = fromSteamNetworkingAddress(info.m_info.m_addrRemote);
				connectionConstructorParameters.steamNetConnection = info.m_hConn;
				connectionConstructorParameters.steamNetworkingSockets = steamNetworkingSockets;
				connectionConstructorParameters.steamListenSocket = acceptingSteamListenSocket;
				std::shared_ptr<Connection2> connection(new Connection2(connectionConstructorParameters));
				initializeSteamNetConnection(*connection);
				connections.push_back(connection);
				incomingConnectionSignal(connection);
				return;
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
