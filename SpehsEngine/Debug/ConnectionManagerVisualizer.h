#pragma once

#include "SpehsEngine/Core/StaticRingBuffer.h"
#include "SpehsEngine/Net/ConnectionId.h"


namespace se
{
	namespace debug
	{
		class ConnectionManagerVisualizer
		{
		public:

			ConnectionManagerVisualizer(net::ConnectionManager& ConnectionManager, imgui::BackendWrapper& imGuiBackendWrapper, const bool enabled);

			void setEnabled(const bool enabled);
			bool getEnabled() const { return preRenderConnection.connected(); }

		private:

			struct ConnectionState
			{
				std::pair<se::time::Time, uint64_t> prevReliableBytesSent;
				std::pair<se::time::Time, uint64_t> prevReliableBytesReceived;
				se::time::Time prevSendQuotaPerSecondTime;
				StaticRingBuffer<float, 64> reliableBytesSentHistory;
				StaticRingBuffer<float, 64> reliableBytesReceivedHistory;
				StaticRingBuffer<float, 64> sendQuotaPerSecondHistory;
			};

			void render();

			net::ConnectionManager& connectionManager;
			imgui::BackendWrapper& imGuiBackendWrapper;

			std::unordered_map<se::net::ConnectionId, ConnectionState> connectionStates;
			boost::signals2::scoped_connection preRenderConnection;
		};
	}
}
