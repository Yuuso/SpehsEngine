#pragma once

#include "SpehsEngine/Net/Connection.h"
#include "SpehsEngine/Net/ConnectionSimulationSettings.h"
#include "SpehsEngine/Net/Endpoint.h"


namespace se
{
	namespace net
	{
		class ConnectionManager
		{
		public:

			ConnectionManager(const std::string_view name);
			~ConnectionManager();

			void update();

			// Accept connections over IP. May require port forwarding depending on where the client is connecting from.
			bool startAcceptingIP(const std::optional<Port> port = std::nullopt, const std::optional<Endpoint> signalingServerEndpoint = std::nullopt);
			void stopAcceptingIP();
			std::optional<Port> getAcceptingPort() const;

			// Accept connections through se::net::SignalingServer.
			bool startAcceptingP2P(const Endpoint& signalingServerEndpoint);
			void stopAcceptingP2P();

			// Connect to a ConnectionManager that has called startAcceptingIP().
			[[nodiscard]] std::shared_ptr<Connection> connectIP(const Endpoint& endpoint, const bool symmetric, const std::string_view name = "", const Time timeout = Time::fromSeconds(10.0f));

			// Connect to a ConnectionManager that has called startAcceptingP2P().
			[[nodiscard]] std::shared_ptr<Connection> connectP2P(const NetIdentity& peerNetIdentity, const Endpoint& signalingServerEndpoint, const std::string_view name = "", const Time timeout = Time::fromSeconds(10.0f));

			void connectToIncomingConnectionSignal(ScopedConnection& scopedConnection, const std::function<void(std::shared_ptr<Connection>&)>& callback);
			void setRemoveUnreferencedConnections(const bool remove) { removeUnreferencedConnections = remove; }

			void setDebugLogLevel(int level) { debugLogLevel = level; }
			int getDebugLogLevel() const { return debugLogLevel; }

			// Connection simulation settings are technically limited to being global among all connection managers
			static void setConnectionSimulationSettings(const ConnectionSimulationSettings& _connectionSimulationSettings);
			static ConnectionSimulationSettings getConnectionSimulationSettings();

			std::vector<std::shared_ptr<Connection>>& getConnections() { return connections; }
			const std::vector<std::shared_ptr<Connection>>& getConnections() const { return connections; }

			const std::string name;

		private:

			struct State;

			void initializeSteamNetConnection(Connection& connection);
			void processIncomingConnectionQueue();
			void processConnectionStatusChangeQueue();
			void closeUnusedSteamListenSockets();

			std::unique_ptr<State> state;
			std::vector<std::shared_ptr<Connection>> connections;
			int debugLogLevel = 0;
			bool removeUnreferencedConnections = false;
		};
	}
}
