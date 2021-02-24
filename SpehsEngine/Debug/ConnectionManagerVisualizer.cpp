#include "stdafx.h"
#include "SpehsEngine/Debug/ConnectionManagerVisualizer.h"

#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"
#include "SpehsEngine/Net/ConnectionManager.h"
#include "SpehsEngine/Net/Connection.h"


namespace se
{
	namespace debug
	{
		ConnectionManagerVisualizer::ConnectionManagerVisualizer(net::ConnectionManager& _connectionManager, imgui::BackendWrapper& _imGuiBackendWrapper, const bool _enabled)
			: connectionManager(_connectionManager)
			, imGuiBackendWrapper(_imGuiBackendWrapper)
		{
			setEnabled(_enabled);
		}

		void ConnectionManagerVisualizer::setEnabled(const bool enabled)
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
				imGuiBackendWrapper.connectToPreRenderSignal(preRenderConnection, boost::bind(&ConnectionManagerVisualizer::render, this));
			}
		}

		void ConnectionManagerVisualizer::render()
		{
			if (ImGui::Begin("ConnectionManager visualizer"))
			{
				ImGui::Text("Local endpoint: " + connectionManager.getLocalEndpoint().toString());
				ImGui::Text("Sent bytes: " + std::to_string(connectionManager.getSentBytes()));
				ImGui::Text("Received bytes: " + std::to_string(connectionManager.getReceivedBytes()));

				const std::vector<std::shared_ptr<net::Connection>> connections = connectionManager.getConnections();
				for (const std::shared_ptr<net::Connection>& connection : connections)
				{
					if (ImGui::CollapsingHeader((connection->debugEndpoint + " \"" + connection->debugName + "\"").c_str()))
					{
						ImGui::Text("Remote endpoint: " + connection->getRemoteEndpoint().address().to_string() + ":" + std::to_string(connection->getRemoteEndpoint().port()));
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
						case net::Connection::Status::Disconnecting: ImGui::Text("Status: Disconnecting"); break;
						case net::Connection::Status::Disconnected: ImGui::Text("Status: Disconnected"); break;
						}
						ImGui::Text("Timeout enabled: " + std::string(connection->getTimeoutEnabled() ? "true" : "false"));
						ImGui::Text("Maximum segment size: " + std::to_string(connection->getMaximumSegmentSize()));
						ImGui::Text("Ping: " + std::to_string(connection->getPing().asMilliseconds()) + " ms");
						ImGui::Text("Average reliable fragment send count: " + std::to_string(connection->getAverageReliableFragmentSendCount()));

						const net::Connection::SentBytes sentBytes = connection->getSentBytes();
						ImGui::Text("Sent bytes (raw): " + std::to_string(sentBytes.raw));
						ImGui::Text("Sent bytes (unreliable): " + std::to_string(sentBytes.unreliable));
						ImGui::Text("Sent bytes (reliable): " + std::to_string(sentBytes.reliable));
						ImGui::Text("Sent bytes (reliable resend): " + std::to_string(sentBytes.reliableResend));
						ImGui::Text("Sent bytes (ack): " + std::to_string(sentBytes.acknowledgement));
						ImGui::Text("Sent bytes (path MSS discovery): " + std::to_string(sentBytes.pathMaximumSegmentSizeDiscovery));
						const net::Connection::ReceivedBytes receivedBytes = connection->getReceivedBytes();
						ImGui::Text("Received bytes (raw): " + std::to_string(receivedBytes.raw));
						ImGui::Text("Received bytes (user): " + std::to_string(receivedBytes.user));

						ImGui::Text("Reliable send quota: " + std::to_string(connection->getReliableSendQuota()));
						ImGui::Text("Reliable send quota per second: " + std::to_string(connection->getSendQuotaPerSecond()));
						ImGui::Text("Unreliable send quota: " + std::to_string(connection->getUnreliableSendQuota()));
						ImGui::Text("Reliable stream offset send: " + std::to_string(connection->getReliableStreamOffsetSend()));
						ImGui::Text("Reliable stream offset receive: " + std::to_string(connection->getReliableStreamOffsetReceive()));
						ImGui::Text("Reliable unacknowledged bytes in queue: " + std::to_string(connection->getReliableUnacknowledgedBytesInQueue()));
						ImGui::Text("Reliable acknowledged bytes in queue: " + std::to_string(connection->getReliableAcknowledgedBytesInQueue()));
					}
				}
			}
			ImGui::End();
		}
	}
}
