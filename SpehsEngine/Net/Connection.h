#pragma once

#include "SpehsEngine/Net/ConnectionId.h"
#include "SpehsEngine/Net/Endpoint.h"


namespace se
{
	namespace net
	{
		struct ConnectionInternalState;

		class Connection
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
				Time ping;
				size_t reliableBytesInSendQueue = 0;
				size_t unreliableBytesInSendQueue = 0;
				size_t sentUnackedReliableBytes = 0;
			};

			struct Settings
			{
				size_t sendBufferSize = 524288;
				Time timeout = Time::fromSeconds(10.0f);
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

			~Connection();

			void update();

			void setPacketReceivingEnabled(const bool _enabled);
			bool getPacketReceivingEnabled() const;

			bool sendPacket(const std::vector<uint8_t>& _data, const bool _reliable);
			bool sendPacket(const BinaryWriter& _binaryWriter, const bool _reliable);

			void disconnect(const std::string& _reason = "");

			void setSettings(const Settings& _settings);
			void setReceiveHandler(const std::function<void(BinaryReader&, const bool)>& _receiveHandler = std::function<void(BinaryReader&, const bool)>());
			void setEnableAssertOnSendFail(const bool enable) { enableAssertOnSendFail = enable; }

			void connectToStatusChangedSignal(ScopedConnection& _scopedConnection, const std::function<void(const Status oldStatus, const Status newStatus)>& _callback);

			Status getStatus() const { return status; }
			DetailedStatus getDetailedStatus() const;
			const Settings& getSettings() const { return settings; }
			const Statistics& getStatistics() const { return statistics; }
			inline bool isConnecting() const { return getStatus() == Status::Connecting; }
			inline bool isConnected() const { return getStatus() == Status::Connected; }
			inline bool isDisconnected() const { return getStatus() == Status::Disconnected; }
			inline bool isP2P() const { return p2p; }
			Time getPing() const;
			uint16_t getMaximumSegmentSize() const;

			const std::string name;
			const ConnectionId connectionId;
			const EstablishmentType establishmentType;
			const Endpoint remoteEndpoint;
			const std::optional<Port> localListeningPort;
			const bool p2p;

		private:

			struct ReceivedPacket
			{
				std::vector<uint8_t> data;
				bool reliable = false;
			};

			static void closeConnectionImpl(const uint32_t _steamNetConnection, const std::string& reason, const bool enableLinger);

			friend class ConnectionManager;

			Connection(const ConnectionParameters& constructorParameters);

			void disconnectImpl(const std::string& reason, const bool enableLinger);
			void setStatus(const Status _status);
			void setSettingsImpl(const Settings& _settings, const bool forceUpdate);
			void receivePacket(const void* data, const size_t size, const bool reliable);

			std::unique_ptr<ConnectionInternalState> state;
			Status status = Connection::Status::Connecting; // Every connection begins in the connecting state
			Settings settings;
			Statistics statistics;
			std::vector<ReceivedPacket> receivedPackets;
			bool enableAssertOnSendFail = true;
			bool packetReceivingEnabled = true;
		};
	}
}
