#pragma once

#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/Connection2.h"
#include "steam/steamnetworkingsockets.h"
#include "boost/signals2/signal.hpp"


namespace se
{
	namespace net
	{
		struct Connection2Parameters
		{
			ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
			std::string name;
			Endpoint remoteEndpoint;
			std::optional<Port> localListeningPort;
			HSteamNetConnection steamNetConnection;
			HSteamListenSocket steamListenSocket; // Connection may be bound to a listening socket
			Connection2::EstablishmentType establishmentType = Connection2::EstablishmentType::Incoming;
			Connection2::Status status = Connection2::Status::Connecting;
			bool p2p = false;
		};

		struct Connection2InternalState
		{
			Connection2InternalState(const Connection2Parameters& parameters)
				: steamNetworkingSockets(*parameters.steamNetworkingSockets)
				, steamNetConnection(parameters.steamNetConnection)
				, steamListenSocket(parameters.steamListenSocket)
			{
			}
			ISteamNetworkingSockets& steamNetworkingSockets;
			HSteamNetConnection steamNetConnection = k_HSteamNetConnection_Invalid;
			HSteamNetConnection closedSteamNetConnection = k_HSteamNetConnection_Invalid;
			const HSteamListenSocket steamListenSocket;
			std::function<void(ReadBuffer&, const bool)> receiveHandler;
			boost::signals2::signal<void(const Connection2::Status, const Connection2::Status)> statusChangedSignal;
		};
	}
}
