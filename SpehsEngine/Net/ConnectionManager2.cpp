#include "stdafx.h"
#include "SpehsEngine/Net/ConnectionManager2.h"

#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Net/AddressUtilityFunctions.h"
#include "SpehsEngine/Net/Signaling/SignalingImpl.h"
#include "SpehsEngine/Net/Internal/InternalTypes.h"
#include <mutex>
#include <unordered_set>


namespace se
{
	namespace net
	{
		namespace
		{
			struct ConnectionStatusChange
			{
				HSteamNetConnection steamNetConnection;
				Connection2::Status status = Connection2::Status::Disconnected;
				std::string reason;
			};

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

		struct ConnectionManager2::State
		{
			static void steamNetConnectionStatusChangedCallbackStatic(SteamNetConnectionStatusChangedCallback_t* const info)
			{
				switch (info->m_info.m_eState)
				{
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
						if (connectionManager->state->ownsConnection(info->m_hConn))
						{
							std::lock_guard<std::recursive_mutex> lock2(connectionManager->state->connectionStatusChangeQueueMutex);
							connectionManager->state->connectionStatusChangeQueue.push_back(ConnectionStatusChange());
							connectionManager->state->connectionStatusChangeQueue.back().steamNetConnection = info->m_hConn;
							connectionManager->state->connectionStatusChangeQueue.back().status = Connection2::Status::Disconnected;
							if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_Dead)
							{
								connectionManager->state->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_Dead";
							}
							else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
							{
								connectionManager->state->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_ProblemDetectedLocally";
							}
							else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer)
							{
								connectionManager->state->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_ClosedByPeer";
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
						Connection2::closeConnectionImpl(info->m_hConn, "", true);
						se::log::error("Failed to accept incoming connection from: " + std::string(info->m_info.m_szConnectionDescription) + ". Could not get steam listen socket.");
						return;
					}

					// This is not necessarily an incoming connection. This callback gets made for outgoing connections as well.
					if (!steamListenSocket.value())
					{
						return;
					}

					std::lock_guard<std::mutex> lock(connectionManagersMutex);
					ConnectionManager2* connectionManager = nullptr;
					bool isP2P = false;
					for (ConnectionManager2* const connectionManager2 : connectionManagers)
					{
						std::lock_guard<std::recursive_mutex> lock2(connectionManager2->state->acceptingSteamListenSocketMutex);
						if (connectionManager2->state->acceptingSteamListenSocketIP == steamListenSocket)
						{
							connectionManager = connectionManager2;
							isP2P = false;
							break;
						}
						else if (connectionManager2->state->acceptingSteamListenSocketP2P == steamListenSocket)
						{
							connectionManager = connectionManager2;
							isP2P = true;
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
						Connection2::closeConnectionImpl(info->m_hConn, "", true);
						se::log::error("Failed to accept incoming connection from: " + std::string(info->m_info.m_szConnectionDescription) + ". Error code: " + std::to_string(acceptConnectionResult));
						return;
					}

					se::log::info("Accepted new connection: " + std::string(info->m_info.m_szConnectionDescription));
					Connection2Parameters connectionParameters;
					connectionParameters.status = Connection2::Status::Connected;
					connectionParameters.establishmentType = Connection2::EstablishmentType::Incoming;
					connectionParameters.remoteEndpoint = fromSteamNetworkingAddress(info->m_info.m_addrRemote);
					connectionParameters.p2p = isP2P;
					connectionParameters.steamNetConnection = info->m_hConn;
					connectionParameters.steamNetworkingSockets = SteamNetworkingSockets();
					connectionParameters.steamListenSocket = *steamListenSocket;

					{
						std::lock_guard<std::recursive_mutex> lock2(connectionManager->state->ownedSteamNetConnectionsMutex);
						connectionManager->state->ownedSteamNetConnections.insert(info->m_hConn);
					}
					{
						std::lock_guard<std::recursive_mutex> lock2(connectionManager->state->incomingConnectionQueueMutex);
						connectionManager->state->incomingConnectionQueue.push_back(connectionParameters);
					}
					return;
				}
				case k_ESteamNetworkingConnectionState_FindingRoute:
					// P2P connections will spend a brief time here where they swap addresses and try to find a route
					se::log::info("Finding route: " + std::string(info->m_info.m_szConnectionDescription));
					break;
				case k_ESteamNetworkingConnectionState_Connected:
					std::lock_guard<std::mutex> lock(connectionManagersMutex);
					for (ConnectionManager2* const connectionManager : connectionManagers)
					{
						if (connectionManager->state->ownsConnection(info->m_hConn))
						{
							std::lock_guard<std::recursive_mutex> lock2(connectionManager->state->connectionStatusChangeQueueMutex);
							connectionManager->state->connectionStatusChangeQueue.push_back(ConnectionStatusChange());
							connectionManager->state->connectionStatusChangeQueue.back().steamNetConnection = info->m_hConn;
							connectionManager->state->connectionStatusChangeQueue.back().status = Connection2::Status::Connected;
							connectionManager->state->connectionStatusChangeQueue.back().reason = "k_ESteamNetworkingConnectionState_Connected";
							return;
						}
					}
					log::warning("No connection manager owns the connection: " + std::string(info->m_info.m_szConnectionDescription)); // TODO: fix
					return;
				}
			}

			bool ownsConnection(const HSteamNetConnection steamNetConnection) const
			{
				std::lock_guard<std::recursive_mutex> lock(ownedSteamNetConnectionsMutex);
				return ownedSteamNetConnections.find(steamNetConnection) != ownedSteamNetConnections.end();
			}

			ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
			std::vector<HSteamListenSocket> steamListenSocketsIP;
			std::vector<HSteamListenSocket> steamListenSocketsP2P;
			HSteamNetPollGroup steamNetPollGroup;
			boost::signals2::signal<void(std::shared_ptr<Connection2>&)> incomingConnectionSignal;

			mutable std::recursive_mutex acceptingSteamListenSocketMutex;
			HSteamListenSocket acceptingSteamListenSocketIP = k_HSteamListenSocket_Invalid;
			HSteamListenSocket acceptingSteamListenSocketP2P = k_HSteamListenSocket_Invalid;
			Endpoint acceptingSignalingServerEndpoint;

			mutable std::recursive_mutex incomingConnectionQueueMutex;
			std::vector<Connection2Parameters> incomingConnectionQueue;

			mutable std::recursive_mutex connectionStatusChangeQueueMutex;
			std::vector<ConnectionStatusChange> connectionStatusChangeQueue;

			mutable std::recursive_mutex ownedSteamNetConnectionsMutex;
			std::unordered_set<HSteamNetConnection> ownedSteamNetConnections;
		};

		// ...

		ConnectionManager2::ConnectionManager2(const std::string_view _name)
			: name(_name)
			, state(new State())
		{
			state->steamNetworkingSockets = SteamNetworkingSockets();
			se_assert(state->steamNetworkingSockets);
			if (state->steamNetworkingSockets)
			{
				state->steamNetPollGroup = state->steamNetworkingSockets->CreatePollGroup();
				if (state->steamNetPollGroup == k_HSteamNetPollGroup_Invalid)
				{
					se::log::error("Failed to create steam net poll group.");
				}
			}

			std::lock_guard<std::mutex> lock(connectionManagersMutex);
			connectionManagers.push_back(this);
		}

		ConnectionManager2::~ConnectionManager2()
		{
			stopAcceptingIP();
			stopAcceptingP2P();
			processIncomingConnectionQueue();
			processConnectionStatusChangeQueue();
			closeUnusedSteamListenSockets();
			for (std::shared_ptr<Connection2>& connection : connections)
			{
				connection->disconnect();
			}
			closeUnusedSteamListenSockets();
			if (state->steamNetworkingSockets)
			{
				if (state->steamNetPollGroup != k_HSteamNetPollGroup_Invalid)
				{
					state->steamNetworkingSockets->DestroyPollGroup(state->steamNetPollGroup);
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
			if (OutConnectionSignaling::staticMutex.try_lock())
			{
				for (std::pair<const Endpoint, std::unique_ptr<SocketTCP>>& pair : OutConnectionSignaling::staticSockets)
				{
					pair.second->update();
				}
				OutConnectionSignaling::staticMutex.unlock();
			}

			if (AcceptorP2P::staticMutex.try_lock())
			{
				for(std::unordered_map<Endpoint, std::unique_ptr<AcceptorP2P>>::iterator it = AcceptorP2P::staticContainer.begin(); it != AcceptorP2P::staticContainer.end(); it++)
				{
					AcceptorP2P& acceptor = *it->second;
					if (!acceptor.socket.hasOnReceiveCallback())
					{
						acceptor.socket.setOnReceiveCallback(std::bind(&AcceptorP2P::receiveHandler, &acceptor, std::placeholders::_1));
					}
					acceptor.socket.update();
					if (!acceptor.socket.isConnected())
					{
						AcceptorP2P::eraseFromStaticContainer(it->first);
					}
				}
				AcceptorP2P::staticMutex.unlock();
			}

			if (state->steamNetworkingSockets)
			{
				// Run GLOBAL callbacks
				// If multiple connection managers exist, this gets called more often but that probably doesn't matter.
				state->steamNetworkingSockets->RunCallbacks();

				processIncomingConnectionQueue();
				processConnectionStatusChangeQueue();

				const time::Time beginTime = time::now();
				const time::Time maxTime = time::fromSeconds(1.0f / 120.0f);
				while (state->steamNetPollGroup != k_HSteamNetPollGroup_Invalid && (time::now() - beginTime <= maxTime))
				{
					ISteamNetworkingMessage* steamNetworkingMessages[16];
					int messageCount = state->steamNetworkingSockets->ReceiveMessagesOnPollGroup(state->steamNetPollGroup, steamNetworkingMessages, 16);
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
								if (connection->state->steamNetConnection == steamNetworkingMessages[m]->m_conn)
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
					if (connections[i]->state->closedSteamNetConnection != k_HSteamNetConnection_Invalid)
					{
						std::lock_guard<std::recursive_mutex> lock(state->ownedSteamNetConnectionsMutex);
						const std::unordered_set<HSteamNetConnection>::iterator it = state->ownedSteamNetConnections.find(connections[i]->state->closedSteamNetConnection);
						se_assert(it != state->ownedSteamNetConnections.end());
						state->ownedSteamNetConnections.erase(it);
						connections[i]->state->closedSteamNetConnection = k_HSteamNetConnection_Invalid;
					}

					if (connections[i]->getStatus() == Connection2::Status::Disconnected)
					{
						std::swap(connections[i], connections.back());
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
			std::vector<Connection2Parameters> temp;
			{
				std::lock_guard<std::recursive_mutex> lock(state->incomingConnectionQueueMutex);
				std::swap(temp, state->incomingConnectionQueue);
			}
			for (const Connection2Parameters& constructorParameters : temp)
			{
				{
					std::lock_guard<std::recursive_mutex> lock(state->acceptingSteamListenSocketMutex);
					if (constructorParameters.p2p
						? (state->acceptingSteamListenSocketP2P == k_HSteamListenSocket_Invalid)
						: (state->acceptingSteamListenSocketIP == k_HSteamListenSocket_Invalid))
					{
						break;
					}
				}
				std::shared_ptr<Connection2> connection(new Connection2(constructorParameters));
				initializeSteamNetConnection(*connection);
				connections.push_back(connection);
				state->incomingConnectionSignal(connection);
			}
		}

		void ConnectionManager2::processConnectionStatusChangeQueue()
		{
			std::vector<ConnectionStatusChange> temp;
			{
				std::lock_guard<std::recursive_mutex> lock(state->connectionStatusChangeQueueMutex);
				std::swap(temp, state->connectionStatusChangeQueue);
			}

			// If there are multiple changes for the same connection, erase all but the latest
			for (size_t i1 = 0; i1 < temp.size(); i1++)
			{
				for (size_t i2 = i1 + 1; i2 < temp.size(); i2++)
				{
					if (temp[i1].steamNetConnection == temp[i2].steamNetConnection)
					{
						temp.erase(temp.begin() + i1);
						i1--;
						break;
					}
				}
			}

			for (const ConnectionStatusChange& connectionStatusChange : temp)
			{
				for (size_t i = 0; i < connections.size(); i++)
				{
					if (connections[i]->state->steamNetConnection == connectionStatusChange.steamNetConnection)
					{
						connections[i]->setStatus(connectionStatusChange.status);
						switch (connectionStatusChange.status)
						{
						case Connection2::Status::Connecting:
							break;
						case Connection2::Status::Connected:
							break;
						case Connection2::Status::Disconnected:
							connections[i]->disconnectImpl(connectionStatusChange.reason, true);
							break;
						}
					}
				}
			}
		}

		void ConnectionManager2::closeUnusedSteamListenSockets()
		{
			auto impl = [this](std::vector<HSteamListenSocket>& steamListenSockets)
			{
				for (size_t i = 0; i < steamListenSockets.size(); i++)
				{
					bool remove = true;
					for (const std::shared_ptr<Connection2>& connection : connections)
					{
						if (connection->state->steamListenSocket == steamListenSockets[i] && !connection->isDisconnected())
						{
							remove = false;
							break;
						}
					}
					if (remove)
					{
						state->steamNetworkingSockets->CloseListenSocket(steamListenSockets[i]);
						steamListenSockets[i] = steamListenSockets.back();
						steamListenSockets.pop_back();
					}
				}
			};
			impl(state->steamListenSocketsIP);
			impl(state->steamListenSocketsP2P);
		}

		bool ConnectionManager2::startAcceptingIP(const std::optional<Port> port, const std::optional<Endpoint> signalingServerEndpoint)
		{
			std::lock_guard<std::recursive_mutex> lock(state->acceptingSteamListenSocketMutex);
			if (state->acceptingSteamListenSocketIP != k_HSteamListenSocket_Invalid)
			{
				if (const std::optional<Port> existingAcceptingPort = getAcceptingPort())
				{
					se::log::error("Already accepting on port: " + existingAcceptingPort->toString());
				}
				return false;
			}

			if (state->steamNetworkingSockets)
			{
				// Check if we already have a listening socket for this port
				if (port)
				{
					SteamNetworkingIPAddr steamNetworkingAddress;
					for (size_t i = 0; i < state->steamListenSocketsIP.size(); i++)
					{
						if (state->steamNetworkingSockets->GetListenSocketAddress(state->steamListenSocketsIP[i], &steamNetworkingAddress))
						{
							if (port->value == steamNetworkingAddress.m_port)
							{
								state->acceptingSteamListenSocketIP = state->steamListenSocketsIP[i];
								state->steamListenSocketsIP[i] = state->steamListenSocketsIP.back();
								state->steamListenSocketsIP.pop_back();
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
				steamNetworkingConfigValues.back().SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)State::steamNetConnectionStatusChangedCallbackStatic);
				HSteamListenSocket steamListenSocket = state->steamNetworkingSockets->CreateListenSocketIP(localSteamNetworkingAddress, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
				if (steamListenSocket != k_HSteamListenSocket_Invalid)
				{
					state->acceptingSteamListenSocketIP = steamListenSocket;
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

		bool ConnectionManager2::startAcceptingP2P(const Endpoint& signalingServerEndpoint)
		{
			{
				std::lock_guard<std::recursive_mutex> lock(state->acceptingSteamListenSocketMutex);
				if (state->acceptingSteamListenSocketP2P != k_HSteamListenSocket_Invalid)
				{
					se::log::error("Already accepting P2P");
					return false;
				}
			}

			if (state->steamNetworkingSockets)
			{
				{
					std::lock_guard<std::recursive_mutex> lock(AcceptorP2P::staticMutex);
					std::unique_ptr<AcceptorP2P>& acceptor = AcceptorP2P::staticContainer[signalingServerEndpoint];
					if (acceptor)
					{
						log::warning("ConnectionManager::startAcceptingP2P: failed to start accepting, another process local ConnectionManager is already accepting from this signaling server.");
						return false;
					}
					acceptor.reset(new AcceptorP2P(signalingServerEndpoint));
					if (!acceptor->socket.isConnected())
					{
						se::log::error("AcceptorP2P failed to connect to signaling server: " + signalingServerEndpoint.toString());
						AcceptorP2P::eraseFromStaticContainer(signalingServerEndpoint);
						return false;
					}
				}

				std::vector<SteamNetworkingConfigValue_t> steamNetworkingConfigValues;
				steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
				steamNetworkingConfigValues.back().SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)State::steamNetConnectionStatusChangedCallbackStatic);
				HSteamListenSocket steamListenSocket = state->steamNetworkingSockets->CreateListenSocketP2P(0, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
				if (steamListenSocket != k_HSteamListenSocket_Invalid)
				{
					std::lock_guard<std::recursive_mutex> lock(state->acceptingSteamListenSocketMutex);
					state->acceptingSteamListenSocketP2P = steamListenSocket;
					state->acceptingSignalingServerEndpoint = signalingServerEndpoint;
					return true;
				}
				else
				{
					se::log::error("Failed to start listening P2P. Failed to create listening socket.");
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		void ConnectionManager2::stopAcceptingIP()
		{
			std::lock_guard<std::recursive_mutex> lock(state->acceptingSteamListenSocketMutex);
			if (state->acceptingSteamListenSocketIP != k_HSteamListenSocket_Invalid)
			{
				state->steamListenSocketsIP.push_back(state->acceptingSteamListenSocketIP);
				state->acceptingSteamListenSocketIP = k_HSteamListenSocket_Invalid;
				closeUnusedSteamListenSockets();
			}
		}

		void ConnectionManager2::stopAcceptingP2P()
		{
			Endpoint acceptingSignalingServerEndpoint2;
			{
				std::lock_guard<std::recursive_mutex> lock(state->acceptingSteamListenSocketMutex);
				if (state->acceptingSteamListenSocketP2P != k_HSteamListenSocket_Invalid)
				{
					state->steamListenSocketsP2P.push_back(state->acceptingSteamListenSocketP2P);
					state->acceptingSteamListenSocketP2P = k_HSteamListenSocket_Invalid;
					acceptingSignalingServerEndpoint2 = state->acceptingSignalingServerEndpoint;
					state->acceptingSignalingServerEndpoint = Endpoint();
					closeUnusedSteamListenSockets();
				}
			}
			if (acceptingSignalingServerEndpoint2)
			{
				AcceptorP2P::eraseFromStaticContainer(acceptingSignalingServerEndpoint2);
			}
		}

		std::optional<Port> ConnectionManager2::getAcceptingPort() const
		{
			std::lock_guard<std::recursive_mutex> lock(state->acceptingSteamListenSocketMutex);
			if (state->acceptingSteamListenSocketIP != k_HSteamListenSocket_Invalid)
			{
				SteamNetworkingIPAddr steamNetworkingAddress;
				if (state->steamNetworkingSockets->GetListenSocketAddress(state->acceptingSteamListenSocketIP, &steamNetworkingAddress))
				{
					return Port(steamNetworkingAddress.m_port);
				}
			}
			return std::nullopt;
		}

		void ConnectionManager2::initializeSteamNetConnection(Connection2& connection)
		{
			if (!state->steamNetworkingSockets->SetConnectionPollGroup(connection.state->steamNetConnection, state->steamNetPollGroup))
			{
				se::log::error("Failed to set connection poll group.");
			}

			std::lock_guard<std::recursive_mutex> lock(state->ownedSteamNetConnectionsMutex);
			state->ownedSteamNetConnections.insert(connection.state->steamNetConnection);
		}

		std::shared_ptr<Connection2> ConnectionManager2::connectIP(const se::net::Endpoint& _endpoint, const bool _symmetric, const std::string_view _name, const time::Time _timeout)
		{
			if (state->steamNetworkingSockets)
			{
				std::vector<SteamNetworkingConfigValue_t> steamNetworkingConfigValues;
				steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
				steamNetworkingConfigValues.back().SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)State::steamNetConnectionStatusChangedCallbackStatic);
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
					std::lock_guard<std::recursive_mutex> lock(state->ownedSteamNetConnectionsMutex);
					steamNetConnection = state->steamNetworkingSockets->ConnectByIPAddress(steamNetworkingAddress, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
					if (steamNetConnection != k_HSteamNetConnection_Invalid)
					{
						state->ownedSteamNetConnections.insert(steamNetConnection);
					}
				}

				if (steamNetConnection == k_HSteamNetConnection_Invalid)
				{
					se::log::error("Failed to connect: " + _endpoint.toString());
					return std::shared_ptr<Connection2>();
				}
				else
				{
					Connection2Parameters connectionParameters;
					connectionParameters.establishmentType = Connection2::EstablishmentType::Outgoing;
					connectionParameters.name = _name;
					connectionParameters.remoteEndpoint = _endpoint;
					connectionParameters.steamNetConnection = steamNetConnection;
					connectionParameters.steamNetworkingSockets = state->steamNetworkingSockets;
					connectionParameters.localListeningPort = getLocalListeningPort(*state->steamNetworkingSockets, steamNetConnection);
					std::shared_ptr<Connection2> connection(new Connection2(connectionParameters));
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

		std::shared_ptr<Connection2> ConnectionManager2::connectP2P(const NetIdentity& _peerNetIdentity, const se::net::Endpoint& _signalingServerEndpoint, const std::string_view _name, const time::Time _timeout)
		{
			se_assert(_peerNetIdentity);
			se_assert(_signalingServerEndpoint);
			se_assert(_peerNetIdentity != getMyNetIdentity() && "Connecting to a process local peer is not supported.");
			if (state->steamNetworkingSockets)
			{
				std::vector<SteamNetworkingConfigValue_t> steamNetworkingConfigValues;
				steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
				steamNetworkingConfigValues.back().SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)State::steamNetConnectionStatusChangedCallbackStatic);
				steamNetworkingConfigValues.push_back(SteamNetworkingConfigValue_t());
				steamNetworkingConfigValues.back().SetInt32(k_ESteamNetworkingConfig_TimeoutInitial, int(_timeout.asMilliseconds()));

				const SteamNetworkingIdentity peerSteamNetworkingIdentity = toSteamNetworkingIdentity(_peerNetIdentity);
				ISteamNetworkingConnectionSignaling* const connectionSignaling = new OutConnectionSignaling(_peerNetIdentity, _signalingServerEndpoint);
				const HSteamNetConnection steamNetConnection = state->steamNetworkingSockets->ConnectP2PCustomSignaling(connectionSignaling, &peerSteamNetworkingIdentity, 0, int(steamNetworkingConfigValues.size()), steamNetworkingConfigValues.data());
				if (steamNetConnection != k_HSteamNetConnection_Invalid)
				{
					std::lock_guard<std::recursive_mutex> lock(state->ownedSteamNetConnectionsMutex);
					state->ownedSteamNetConnections.insert(steamNetConnection);
				}
				else
				{
					se::log::error("Failed to connect: " + _peerNetIdentity.toString());
					return std::shared_ptr<Connection2>();
				}

				Connection2Parameters connectionParameters;
				connectionParameters.establishmentType = Connection2::EstablishmentType::Outgoing;
				connectionParameters.name = _name;
				connectionParameters.steamNetConnection = steamNetConnection;
				connectionParameters.steamNetworkingSockets = state->steamNetworkingSockets;
				connectionParameters.localListeningPort = getLocalListeningPort(*state->steamNetworkingSockets, steamNetConnection);
				SteamNetConnectionInfo_t steamNetConnectionInfo;
				if (state->steamNetworkingSockets->GetConnectionInfo(steamNetConnection, &steamNetConnectionInfo))
				{
					connectionParameters.remoteEndpoint = fromSteamNetworkingAddress(steamNetConnectionInfo.m_addrRemote);
				}
				else
				{
					se::log::error("Failed to get connection info from a new connection");
				}
				std::shared_ptr<Connection2> connection(new Connection2(connectionParameters));
				initializeSteamNetConnection(*connection);
				connections.push_back(connection);
				return connection;
			}
			else
			{
				return std::shared_ptr<Connection2>();
			}
		}

		void ConnectionManager2::connectToIncomingConnectionSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(std::shared_ptr<Connection2>&)>& callback)
		{
			scopedConnection = state->incomingConnectionSignal.connect(callback);
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
