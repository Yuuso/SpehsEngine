#pragma once

#include "boost/bind.hpp"
#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Connection2.h"
#include "steam/steamnetworkingsockets.h"
#include <functional>
#include <unordered_map>
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
			bool startListening(const Port port);
			void stopListening();
			std::shared_ptr<Connection2> connect(const se::net::Endpoint& endpoint);
			void connectToIncomingConnectionSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(std::shared_ptr<Connection2>&)>& callback)
			{
				scopedConnection = incomingConnectionSignal.connect(callback);
			}

			const std::string name;

		private:

			void steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t& info);

			static void steamNetConnectionStatusChangedCallbackStatic(SteamNetConnectionStatusChangedCallback_t* info);

			ConnectionId generateNewConnectionId();

			ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
			HSteamListenSocket steamListenSocket = k_HSteamListenSocket_Invalid;
			HSteamNetPollGroup steamNetPollGroup;
			std::vector<std::shared_ptr<Connection2>> connections;
			boost::signals2::signal<void(std::shared_ptr<Connection2>&)> incomingConnectionSignal;
			static std::recursive_mutex callbackConnectionManagerMutex;
			static ConnectionManager2* callbackConnectionManager;
		};
	}
}
