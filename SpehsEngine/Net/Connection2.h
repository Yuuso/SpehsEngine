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
				Disconnected
			};

			struct DetailedStatus // TODO renamings...
			{
				time::Time ping;
				size_t reliableBytesInSendQueue = 0;
				size_t unreliableBytesInSendQueue = 0;
				size_t sentUnackedReliableBytes = 0;
			};

			struct Settings
			{
				size_t sendBufferSize = 524288;
				time::Time timeout = time::fromSeconds(10.0f);
			};

			struct Statistics
			{
				int64_t sentBytesReliable = 0;
				int64_t sentBytesUnreliable = 0;
				int64_t sentBytesTotal = 0;
				int64_t sentPacketsReliable = 0;
				int64_t sentPacketsUnreliable = 0;
				int64_t sentPacketsTotal = 0;
				uint64_t receivedBytesReliable = 0;
				uint64_t receivedBytesUnreliable = 0;
				uint64_t receivedBytesTotal = 0;
				uint64_t receivedPacketsReliable = 0;
				uint64_t receivedPacketsUnreliable = 0;
				uint64_t receivedPacketsTotal = 0;
			};

			~Connection2();

			void update();

			bool sendPacket(const WriteBuffer& writeBuffer, const bool reliable);

			void disconnect(const std::string& reason = "");

			void setSettings(const Settings& _settings);
			void setReceiveHandler(const std::function<void(ReadBuffer&, const bool)>& _receiveHandler = std::function<void(ReadBuffer&, const bool)>()) { receiveHandler = _receiveHandler; }
			void setEnableAssertOnSendFail(const bool enable) { enableAssertOnSendFail = enable; }

			void connectToStatusChangedSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const Status oldStatus, const Status newStatus)>& callback)
			{
				scopedConnection = statusChangedSignal.connect(callback);
			}

			Status getStatus() const { return status; }
			DetailedStatus getDetailedStatus() const;
			const Settings& getSettings() const { return settings; }
			const Statistics& getStatistics() const { return statistics; }
			inline bool isConnecting() const { return getStatus() == Status::Connecting; }
			inline bool isConnected() const { return getStatus() == Status::Connected; }
			inline bool isDisconnected() const { return getStatus() == Status::Disconnected; }
			time::Time getPing() const;
			uint16_t getMaximumSegmentSize() const;

			const std::string name;
			const ConnectionId connectionId;
			const EstablishmentType establishmentType;
			const se::net::Endpoint remoteEndpoint;

		private:

			struct ReceivedPacket
			{
				std::vector<uint8_t> data;
				bool reliable = false;
			};

			struct ConstructorParameters
			{
				ISteamNetworkingSockets* steamNetworkingSockets = nullptr;
				std::string name;
				se::net::Endpoint remoteEndpoint;
				HSteamNetConnection steamNetConnection;
				HSteamListenSocket steamListenSocket; // Connection may be bound to a listening socket
				EstablishmentType establishmentType = EstablishmentType::Incoming;
			};

			static void closeConnectionImpl(const HSteamNetConnection _steamNetConnection, const std::string& reason, const bool enableLinger);

			void steamNetConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t& info);

			friend class ConnectionManager2;

			Connection2(const ConstructorParameters& constructorParameters);

			void disconnectImpl(const std::string& reason, const bool enableLinger);
			void setStatus(const Status _status);
			void setSettingsImpl(const Settings& _settings, const bool forceUpdate);
			void receivePacket(const void* data, const size_t size, const bool reliable);

			ISteamNetworkingSockets& steamNetworkingSockets;
			HSteamNetConnection steamNetConnection = k_HSteamNetConnection_Invalid;
			HSteamNetConnection closedSteamNetConnection = k_HSteamNetConnection_Invalid;
			HSteamNetConnection localRemoteSteamNetConnection = k_HSteamNetConnection_Invalid;
			const HSteamListenSocket steamListenSocket;
			std::shared_ptr<bool> localConnectionLifeline;
			Status status = Status::Connecting; // Every connection begins in the connecting state
			Settings settings;
			Statistics statistics;
			std::vector<ReceivedPacket> receivedPackets;
			std::function<void(ReadBuffer&, const bool)> receiveHandler;
			boost::signals2::signal<void(const Status, const Status)> statusChangedSignal;
			bool enableAssertOnSendFail = true;
		};
	}
}
