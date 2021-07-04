#include "stdafx.h"
#include "SpehsEngine/Debug/ConnectionManager2Visualizer.h"

#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"
#include "SpehsEngine/Net/ConnectionManager2.h"
#include "SpehsEngine/Net/Connection2.h"


namespace se
{
	namespace debug
	{
		ConnectionManager2Visualizer::ConnectionManager2Visualizer(net::ConnectionManager2& _connectionManager2, imgui::BackendWrapper& _imGuiBackendWrapper, const bool _enabled)
			: connectionManager2(_connectionManager2)
			, imGuiBackendWrapper(_imGuiBackendWrapper)
		{
			setEnabled(_enabled);
		}

		void ConnectionManager2Visualizer::setEnabled(const bool enabled)
		{
			if (preRenderConnection.connected() == enabled)
			{
				return;
			}

			if (preRenderConnection.connected())
			{
				preRenderConnection.disconnect();
			}
			else
			{
				imGuiBackendWrapper.connectToPreRenderSignal(preRenderConnection, boost::bind(&ConnectionManager2Visualizer::render, this));
			}
		}

		void ConnectionManager2Visualizer::render()
		{
			if (ImGui::Begin("ConnectionManager2 visualizer"))
			{
				int debugLogLevel = connectionManager2.getDebugLogLevel();
				if (ImGui::InputT("Debug log level", debugLogLevel))
				{
					connectionManager2.setDebugLogLevel(debugLogLevel);
				}

				if (const std::optional<net::Port> acceptingPort = connectionManager2.getAcceptingPort())
				{
					ImGui::Text("Accepting port: " + acceptingPort->toString());
				}

				if (ImGui::CollapsingHeader("Global connection simulation settings"))
				{
					net::ConnectionSimulationSettings connectionSimulationSettings = net::ConnectionManager2::getConnectionSimulationSettings();
					bool connectionSimulationSettingsChanged = false;
					connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to re-order incoming packet", connectionSimulationSettings.chanceToReorderIncoming, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
					connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to re-order outgoing packet", connectionSimulationSettings.chanceToReorderOutgoing, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
					connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to drop incoming", connectionSimulationSettings.chanceToDropIncoming, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
					connectionSimulationSettingsChanged = ImGui::DragScalar1("Chance to drop outgoing", connectionSimulationSettings.chanceToDropOutgoing, 0.001f, 0.0f, 1.0f) || connectionSimulationSettingsChanged;
					connectionSimulationSettingsChanged = ImGui::InputT("Delay incoming", connectionSimulationSettings.delayIncoming) || connectionSimulationSettingsChanged;
					connectionSimulationSettingsChanged = ImGui::InputT("Delay outgoing", connectionSimulationSettings.delayOutgoing) || connectionSimulationSettingsChanged;
					if (connectionSimulationSettingsChanged)
					{
						net::ConnectionManager2::setConnectionSimulationSettings(connectionSimulationSettings);
					}
				}

				const std::vector<std::shared_ptr<net::Connection2>> connections = connectionManager2.getConnections();
				for (const std::shared_ptr<net::Connection2>& connection : connections)
				{
					if (ImGui::CollapsingHeader(connection->remoteEndpoint.toString().c_str()))
					{
						const se::time::Time now = se::time::now();
						const se::time::Time historyRecordInterval = se::time::fromSeconds(0.1f);
						ConnectionState& connectionState = connectionStates[connection->connectionId];

						ImGui::Text("Remote endpoint: " + connection->remoteEndpoint.toString());
						ImGui::Text("Connection id: " + std::to_string(connection->connectionId.value));
						switch (connection->establishmentType)
						{
						case net::Connection2::EstablishmentType::Incoming: ImGui::Text("Establishment type: Incoming"); break;
						case net::Connection2::EstablishmentType::Outgoing: ImGui::Text("Establishment type: Outgoing"); break;
						}
						switch (connection->getStatus())
						{
						case net::Connection2::Status::Connecting: ImGui::Text("Status: Connecting"); break;
						case net::Connection2::Status::Connected: ImGui::Text("Status: Connected"); break;
						case net::Connection2::Status::Disconnected: ImGui::Text("Status: Disconnected"); break;
						}

						const net::Connection2::DetailedStatus detailedStatus = connection->getDetailedStatus();
						ImGui::Text("Bytes in send queue (reliable): " + std::to_string(detailedStatus.reliableBytesInSendQueue));
						ImGui::Text("Bytes in send queue (unreliable): " + std::to_string(detailedStatus.unreliableBytesInSendQueue));
						ImGui::Text("Sent unacked reliable bytes: " + std::to_string(detailedStatus.sentUnackedReliableBytes));

						const net::Connection2::Statistics& statistics = connection->getStatistics();
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
	}
}
