#pragma once

#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/Connection.h"
#include "steam/steamnetworkingsockets.h"


namespace se
{
	namespace net
	{
		struct ConnectionParameters
		{
			ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
			std::string name;
			Endpoint remoteEndpoint;
			std::optional<Port> localListeningPort;
			HSteamNetConnection steamNetConnection;
			HSteamListenSocket steamListenSocket; // Connection may be bound to a listening socket
			Connection::EstablishmentType establishmentType = Connection::EstablishmentType::Incoming;
			Connection::Status status = Connection::Status::Connecting;
			bool p2p = false;
		};

		struct ConnectionInternalState
		{
			ConnectionInternalState(const ConnectionParameters& parameters)
				: steamNetworkingSockets(*parameters.steamNetworkingSockets)
				, steamNetConnection(parameters.steamNetConnection)
				, steamListenSocket(parameters.steamListenSocket)
			{
			}
			ISteamNetworkingSockets& steamNetworkingSockets;
			HSteamNetConnection steamNetConnection = k_HSteamNetConnection_Invalid;
			HSteamNetConnection closedSteamNetConnection = k_HSteamNetConnection_Invalid;
			const HSteamListenSocket steamListenSocket;
			std::function<void(BinaryReader&, const bool)> receiveHandler;
			boost::signals2::signal<void(const Connection::Status, const Connection::Status)> statusChangedSignal;
		};
	}
}
