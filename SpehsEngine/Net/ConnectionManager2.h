#pragma once

#include "boost/bind.hpp"
#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Connection2.h"
#include "SpehsEngine/Net/Connection.h"
#include "steam/steamnetworkingsockets.h"
#include <unordered_map>
#include <unordered_set>
#include <mutex>


namespace se
{
	namespace net
	{
		class SocketTCP;
		struct NetIdentity;

		class ConnectionManager2
		{
		public:

			ConnectionManager2(const std::string_view name);
			~ConnectionManager2();

			void update();

			// Accept connections over IP. May require port forwarding depending on where the client is connecting from.
			bool startAcceptingIP(const std::optional<Port> port = std::nullopt, const std::optional<Endpoint> signalingServerEndpoint = std::nullopt);
			void stopAcceptingIP();
			std::optional<Port> getAcceptingPort() const;

			// Accept connections through se::net::SignalingServer.
			bool startAcceptingP2P(const Endpoint& signalingServerEndpoint);
			void stopAcceptingP2P();

			// Connect to a ConnectionManager that has called startAcceptingIP().
			[[nodiscard]] std::shared_ptr<Connection2> connectIP(const se::net::Endpoint& endpoint, const bool symmetric, const std::string_view name = "", const time::Time timeout = time::fromSeconds(10.0f));

			// Connect to a ConnectionManager that has called startAcceptingP2P().
			[[nodiscard]] std::shared_ptr<Connection2> connectP2P(const NetIdentity& peerNetIdentity, const se::net::Endpoint& signalingServerEndpoint, const std::string_view name = "", const time::Time timeout = time::fromSeconds(10.0f));

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

			struct ConnectionStatusChange
			{
				HSteamNetConnection steamNetConnection;
				Connection2::Status status = Connection2::Status::Disconnected;
				std::string reason;
			};

			static void steamNetConnectionStatusChangedCallbackStatic(SteamNetConnectionStatusChangedCallback_t* const info);

			void initializeSteamNetConnection(Connection2& connection);
			void processIncomingConnectionQueue();
			void processConnectionStatusChangeQueue();
			void closeUnusedSteamListenSockets();
			bool ownsConnection(const HSteamNetConnection steamNetConnection) const;

			ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
			std::vector<HSteamListenSocket> steamListenSocketsIP;
			std::vector<HSteamListenSocket> steamListenSocketsP2P;
			HSteamNetPollGroup steamNetPollGroup;
			int debugLogLevel = 0;
			bool removeUnreferencedConnections = false;
			std::vector<std::shared_ptr<Connection2>> connections;
			boost::signals2::signal<void(std::shared_ptr<Connection2>&)> incomingConnectionSignal;

			mutable std::recursive_mutex acceptingSteamListenSocketMutex;
			HSteamListenSocket acceptingSteamListenSocketIP = k_HSteamListenSocket_Invalid;
			HSteamListenSocket acceptingSteamListenSocketP2P = k_HSteamListenSocket_Invalid;
			Endpoint acceptingSignalingServerEndpoint;

			mutable std::recursive_mutex incomingConnectionQueueMutex;
			std::vector<Connection2::ConstructorParameters> incomingConnectionQueue;

			mutable std::recursive_mutex connectionStatusChangeQueueMutex;
			std::vector<ConnectionStatusChange> connectionStatusChangeQueue;

			mutable std::recursive_mutex ownedSteamNetConnectionsMutex;
			std::unordered_set<HSteamNetConnection> ownedSteamNetConnections;
		};
	}
}
