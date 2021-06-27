#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Net/Endpoint.h"
#include "SpehsEngine/Net/SocketUDP2.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StrongInt.h"
#include "SpehsEngine/Core/LockGuard.h"
#include "SpehsEngine/Net/ConnectionPackets.h"
#include "steam/steamnetworkingsockets.h"
#include <functional>
#include <map>
#include <mutex>


namespace se
{
	class WriteBuffer;
	class ReadBuffer;
	namespace net
	{
		class ConnectionManager2;
		class Connection2
		{
		public:

			enum class EstablishmentType
			{
				Outgoing,
				Incoming
			};

			enum class Status
			{
				Connecting,
				Connected,
				Disconnecting,
				Disconnected
			};

			struct Statistics
			{
				int64_t sentBytesReliable = 0;
				int64_t sentBytesUnreliable = 0;
				int64_t sentBytesTotal = 0;
				int64_t sentPacketsReliable = 0;
				int64_t sentPacketsUnreliable = 0;
				int64_t sentPacketsTotal = 0;
			};

			~Connection2();

			void update();

			bool sendPacket(const WriteBuffer& writeBuffer, const bool reliable);

			void disconnect(const std::string& reason = "");

			void setReceiveHandler(const std::function<void(ReadBuffer&, const bool)>& _receiveHandler)
			{
				receiveHandler = _receiveHandler;
			}

			void connectToStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const Status oldStatus, const Status newStatus)>& callback)
			{
				scopedConnection = statusChangedSignal.connect(callback);
			}

			Status getStatus() const { return status; }
			const Statistics& getStatistics() const { return statistics; }
			inline bool isConnecting() const { return getStatus() == Status::Connecting; }
			inline bool isConnected() const { return getStatus() == Status::Connected; }
			inline bool isDisconnecting() const { return getStatus() == Status::Disconnecting; }
			inline bool isDisconnected() const { return getStatus() == Status::Disconnected; }

			const EstablishmentType establishmentType;
			const ConnectionId connectionId;
			const se::net::Endpoint remoteEndpoint;

		private:

			struct ReceivedPacket
			{
				std::vector<uint8_t> data;
				bool reliable = false;
			};

			void steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t& info);

			friend class ConnectionManager2;

			Connection2(ISteamNetworkingSockets& _steamNetworkingSockets, const ConnectionId _connectionId, const se::net::Endpoint& _remoteEndpoint,
				const HSteamNetConnection _steamNetConnection, const EstablishmentType _establishmentType);

			void disconnectImpl(const std::string& reason, const bool enableLinger);
			void setStatus(const Status _status);
			void receivePacket(const void* data, const size_t size, const bool reliable);

			ISteamNetworkingSockets& steamNetworkingSockets;
			HSteamNetConnection steamNetConnection;
			Status status = Status::Connecting; // Every connection begins in the connecting state
			Statistics statistics;
			std::vector<ReceivedPacket> receivedPackets;
			std::function<void(ReadBuffer&, const bool)> receiveHandler;
			boost::signals2::signal<void(const Status, const Status)> statusChangedSignal;
		};
	}
}
