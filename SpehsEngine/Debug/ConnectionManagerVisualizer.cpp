#include "stdafx.h"
#include "SpehsEngine/Debug/ConnectionManagerVisualizer.h"

#include "SpehsEngine/Core/StaticRingBuffer.h"
#include "SpehsEngine/Net/ConnectionId.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"
#include "SpehsEngine/Net/ConnectionManager.h"
#include "SpehsEngine/Net/Connection.h"


namespace se
{
	namespace debug
	{
		struct ConnectionState
		{
			std::pair<Time, uint64_t> prevReliableBytesSent;
			std::pair<Time, uint64_t> prevReliableBytesReceived;
			Time prevSendQuotaPerSecondTime;
			StaticRingBuffer<float, 64> reliableBytesSentHistory;
			StaticRingBuffer<float, 64> reliableBytesReceivedHistory;
			StaticRingBuffer<float, 64> sendQuotaPerSecondHistory;
		};

		struct ConnectionManagerVisualizer : public IConnectionManagerVisualizer
		{
			ConnectionManagerVisualizer(net::ConnectionManager& _connectionManager, imgui::BackendWrapper& _imGuiBackendWrapper, const bool _enabled)
				: connectionManager(_connectionManager)
				, imGuiBackendWrapper(_imGuiBackendWrapper)
			{
				setEnabled(_enabled);
			}

			void setEnabled(const bool enabled) final
			{
				if (preRenderConnection.isConnected() == enabled)
				{
					return;
				}

				if (preRenderConnection)
				{
					preRenderConnection.disconnect();
				}
				else
				{
					imGuiBackendWrapper.connectToPreRenderSignal(preRenderConnection, std::bind(&ConnectionManagerVisualizer::render, this));
				}
			}

			void render()
			{
				if (ImGui::Begin("ConnectionManager visualizer"))
				{
					int debugLogLevel = connectionManager.getDebugLogLevel();
					if (ImGui::InputT("Debug log level", debugLogLevel))
					{
						connectionManager.setDebugLogLevel(debugLogLevel);
					}

					if (const std::optional<net::Port> acceptingPort = connectionManager.getAcceptingPort())
					{
						ImGui::Text("Accepting port: " + acceptingPort->toString());
					}

					if (ImGui::CollapsingHeader("Global connection simulation settings"))
					{
						net::ConnectionSimulationSettings connectionSimulationSettings = net::ConnectionManager::getConnectionSimulationSettings();
						bool connectionSimulationSettingsChanged = false;
						connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to re-order incoming packet", connectionSimulationSettings.chanceToReorderIncoming, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
						connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to re-order outgoing packet", connectionSimulationSettings.chanceToReorderOutgoing, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
						connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to drop incoming", connectionSimulationSettings.chanceToDropIncoming, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
						connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to drop outgoing", connectionSimulationSettings.chanceToDropOutgoing, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
						connectionSimulationSettingsChanged = ImGui::InputT("Delay incoming", connectionSimulationSettings.delayIncoming) || connectionSimulationSettingsChanged;
						connectionSimulationSettingsChanged = ImGui::InputT("Delay outgoing", connectionSimulationSettings.delayOutgoing) || connectionSimulationSettingsChanged;
						if (connectionSimulationSettingsChanged)
						{
							net::ConnectionManager::setConnectionSimulationSettings(connectionSimulationSettings);
						}
					}

					const std::vector<std::shared_ptr<net::Connection>> connections = connectionManager.getConnections();
					for (const std::shared_ptr<net::Connection>& connection : connections)
					{
						if (ImGui::CollapsingHeader(connection->remoteEndpoint.toString().c_str()))
						{
							const Time now = se::getEpochTime();
							const Time historyRecordInterval = se::Time::fromSeconds(0.1f);
							ConnectionState& connectionState = connectionStates[connection->connectionId];

							ImGui::Text("Remote endpoint: " + connection->remoteEndpoint.toString());
							ImGui::Text("Connection id: " + std::to_string(connection->connectionId.value));
							switch (connection->establishmentType)
							{
							case net::Connection::EstablishmentType::Incoming: ImGui::Text("Establishment type: Incoming"); break;
							case net::Connection::EstablishmentType::Outgoing: ImGui::Text("Establishment type: Outgoing"); break;
							}
							switch (connection->getStatus())
							{
							case net::Connection::Status::Connecting: ImGui::Text("Status: Connecting"); break;
							case net::Connection::Status::Connected: ImGui::Text("Status: Connected"); break;
							case net::Connection::Status::Disconnected: ImGui::Text("Status: Disconnected"); break;
							}

							const net::Connection::DetailedStatus detailedStatus = connection->getDetailedStatus();
							ImGui::Text("Bytes in send queue (reliable): " + std::to_string(detailedStatus.reliableBytesInSendQueue));
							ImGui::Text("Bytes in send queue (unreliable): " + std::to_string(detailedStatus.unreliableBytesInSendQueue));
							ImGui::Text("Sent unacked reliable bytes: " + std::to_string(detailedStatus.sentUnackedReliableBytes));

							const net::Connection::Statistics& statistics = connection->getStatistics();
							ImGui::Text("Sent bytes (reliable): " + std::to_string(statistics.sentBytesReliable));
							ImGui::Text("Sent bytes (unreliable): " + std::to_string(statistics.sentBytesUnreliable));
							ImGui::Text("Sent bytes (total): " + std::to_string(statistics.sentBytesTotal));
							ImGui::Text("Sent packets (reliable): " + std::to_string(statistics.sentPacketsReliable));
							ImGui::Text("Sent packets (unreliable): " + std::to_string(statistics.sentPacketsUnreliable));
							ImGui::Text("Sent packets (total): " + std::to_string(statistics.sentPacketsTotal));
							ImGui::Text("Received bytes (reliable): " + std::to_string(statistics.receivedBytesReliable));
							ImGui::Text("Received bytes (unreliable): " + std::to_string(statistics.receivedBytesUnreliable));
							ImGui::Text("Received bytes (total): " + std::to_string(statistics.receivedBytesTotal));
							ImGui::Text("Received packets (reliable): " + std::to_string(statistics.receivedPacketsReliable));
							ImGui::Text("Received packets (unreliable): " + std::to_string(statistics.receivedPacketsUnreliable));
							ImGui::Text("Received packets (total): " + std::to_string(statistics.receivedPacketsTotal));
						}
					}
				}
				ImGui::End();
			}

			bool getEnabled() const final { return preRenderConnection.isConnected(); }

			net::ConnectionManager& connectionManager;
			imgui::BackendWrapper& imGuiBackendWrapper;

			std::unordered_map<net::ConnectionId, ConnectionState> connectionStates;
			ScopedConnection preRenderConnection;
		};

		std::unique_ptr<IConnectionManagerVisualizer> IConnectionManagerVisualizer::create(net::ConnectionManager& _connectionManager, imgui::BackendWrapper& _imGuiBackendWrapper, const bool _enabled)
		{
			return std::unique_ptr<IConnectionManagerVisualizer>(new ConnectionManagerVisualizer(_connectionManager, _imGuiBackendWrapper, _enabled));
		}
	}
}
