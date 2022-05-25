#pragma once

#include "boost/bind.hpp"
#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Connection2.h"
#include "SpehsEngine/Net/Connection.h"
#include "SpehsEngine/Net/IOService.h"
#include "steam/steamnetworkingsockets.h"
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <mutex>


namespace se
{
	namespace net
	{
		class SocketTCP;

		class ConnectionManager2
		{
		public:

			ConnectionManager2(const std::string_view name);
			~ConnectionManager2();

			void update();
			bool startAccepting(const std::optional<Port> port = std::nullopt, const std::optional<Endpoint> signalingServerEndpoint = std::nullopt);
			bool startAcceptingP2P(const Endpoint& signalingServerEndpoint);
			void stopAccepting();
			std::optional<Port> getAcceptingPort() const;
			std::shared_ptr<Connection2> connect(const se::net::Endpoint& endpoint, const bool symmetric, const std::string_view name = "", const time::Time timeout = time::fromSeconds(10.0f));
			std::shared_ptr<Connection2> connectP2P(const se::net::Endpoint& endpoint, const bool symmetric, const std::string_view name = "", const time::Time timeout = time::fromSeconds(10.0f));
			void connectToIncomingConnectionSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(std::shared_ptr<Connection2>&)>& callback)
			{
				scopedConnection = incomingConnectionSignal.connect(callback);
			}
			void setRemoveUnreferencedConnections(const bool remove) { removeUnreferencedConnections = remove; }

			void setDebugLogLevel(int level) { debugLogLevel = level; }
			int getDebugLogLevel() const { return debugLogLevel; }

			// Connection simulation settings are technically limited to being global among all connection managers
			static void setConnectionSimulationSettings(const ConnectionSimulationSettings& _connectionSimulationSettings);
			static ConnectionSimulationSettings getConnectionSimulationSettings();

			std::vector<std::shared_ptr<Connection2>>& getConnections() { return connections; }
			const std::vector<std::shared_ptr<Connection2>>& getConnections() const { return connections; }

			const std::string name;

		private:

			struct ConnectionSignaling;
			struct SignalingReceivedContext;

			struct ConnectionStatusChange
			{
				HSteamNetConnection steamNetConnection;
				Connection2::Status status = Connection2::Status::Disconnected;
				std::string reason;
			};

			static ISteamNetworkingConnectionSignaling* createSignalingForConnectionStatic(const SteamNetworkingIdentity& steamNetworkingIdentity, SteamNetworkingErrMsg& error);
			static void steamNetConnectionStatusChangedCallbackStatic(SteamNetConnectionStatusChangedCallback_t* const info);

			void initializeSteamNetConnection(Connection2& connection);
			void processIncomingConnectionQueue();
			void processConnectionStatusChangeQueue();
			void closeUnusedSteamListenSockets();
			bool ownsConnection(const HSteamNetConnection steamNetConnection) const;

			ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
			std::vector<HSteamListenSocket> steamListenSockets;
			HSteamNetPollGroup steamNetPollGroup;
			int debugLogLevel = 0;
			bool removeUnreferencedConnections = false;
			std::vector<std::shared_ptr<Connection2>> connections;
			boost::signals2::signal<void(std::shared_ptr<Connection2>&)> incomingConnectionSignal;

			mutable std::recursive_mutex acceptingSteamListenSocketMutex;
			HSteamListenSocket acceptingSteamListenSocket = k_HSteamListenSocket_Invalid;
			std::unique_ptr<IOService> acceptingSignalingIoService;
			std::unique_ptr<SocketTCP> acceptingSignalingSocketTCP;

			mutable std::recursive_mutex incomingConnectionQueueMutex;
			std::vector<Connection2::ConstructorParameters> incomingConnectionQueue;

			mutable std::recursive_mutex connectionStatusChangeQueueMutex;
			std::vector<ConnectionStatusChange> connectionStatusChangeQueue;

			mutable std::recursive_mutex ownedSteamNetConnectionsMutex;
			std::unordered_set<HSteamNetConnection> ownedSteamNetConnections;
		};
	}
}
