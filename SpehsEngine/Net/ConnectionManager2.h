#pragma once

#include "boost/bind.hpp"
#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Connection2.h"
#include "SpehsEngine/Net/Connection.h"
#include "steam/steamnetworkingsockets.h"
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <mutex>


namespace se
{
	namespace net
	{
		class ConnectionManager2
		{
		public:

			ConnectionManager2(const std::string_view name);
			~ConnectionManager2();

			void update();
			bool startAccepting(const std::optional<Port> port = std::nullopt);
			void stopAccepting();
			std::optional<Port> getAcceptingPort() const;
			std::shared_ptr<Connection2> connect(const se::net::Endpoint& endpoint, const bool symmetric, const std::string_view name = "", const time::Time timeout = time::fromSeconds(10.0f));
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

			struct LocalRemoteConnection
			{
				HSteamNetConnection fromSteamNetConnection;
				HSteamNetConnection toSteamNetConnection;
				HSteamNetConnection localRemoteSteamNetConnection;
				std::shared_ptr<bool> localConnectionLifeline;
			};

			void steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t& info);
			static void steamNetConnectionStatusChangedCallbackStatic(SteamNetConnectionStatusChangedCallback_t* info);

			void initializeSteamNetConnection(Connection2& connection);
			void processLocalRemoteConnectionQueue();
			void closeUnusedSteamListenSockets();

			ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
			HSteamListenSocket acceptingSteamListenSocket = k_HSteamListenSocket_Invalid;
			std::vector<HSteamListenSocket> steamListenSockets;
			HSteamNetPollGroup steamNetPollGroup;
			int debugLogLevel = 0;
			bool removeUnreferencedConnections = false;
			std::vector<std::shared_ptr<Connection2>> connections;
			boost::signals2::signal<void(std::shared_ptr<Connection2>&)> incomingConnectionSignal;

			std::mutex localRemoteConnectionQueueMutex;
			std::vector<LocalRemoteConnection> localRemoteConnectionQueue;

			std::recursive_mutex ownedSteamNetConnectionsMutex;
			std::unordered_set<HSteamNetConnection> ownedSteamNetConnections;
		};
	}
}
