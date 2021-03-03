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
						const se::time::Time now = se::time::now();
						const se::time::Time historyRecordInterval = se::time::fromSeconds(0.1f);
						ConnectionState& connectionState = connectionStates[connection->connectionId];
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

						// Reliable send
						{
							const uint64_t reliableSentAcknowledgedBytesInQueue = connection->getReliableSentAcknowledgedBytesInQueue();
							ImGui::Text("Reliable acknowledged bytes in queue: " + std::to_string(reliableSentAcknowledgedBytesInQueue));
							ImGui::Text("Reliable unacknowledged bytes in queue: " + std::to_string(connection->getReliableUnacknowledgedBytesInQueue()));
							const uint64_t reliableStreamOffsetSend = connection->getReliableStreamOffsetSend();
							ImGui::Text("Reliable stream offset send: " + std::to_string(reliableStreamOffsetSend));
							const uint64_t reliableAcknowledgedBytesSend = reliableStreamOffsetSend + reliableSentAcknowledgedBytesInQueue;
							if (now - connectionState.prevReliableBytesSent.first >= historyRecordInterval)
							{
								if (connectionState.reliableBytesSentHistory.isFull())
								{
									connectionState.reliableBytesSentHistory.popFront();
								}
								const uint64_t advancement = reliableAcknowledgedBytesSend - connectionState.prevReliableBytesSent.second;
								connectionState.reliableBytesSentHistory.pushBack(float(advancement));
								connectionState.prevReliableBytesSent.first = now;
								connectionState.prevReliableBytesSent.second = reliableAcknowledgedBytesSend;
							}
							float min = std::numeric_limits<float>::max();
							float max = 0.0f;
							float average = 0.0f;
							for (size_t i = 0; i < connectionState.reliableBytesSentHistory.getSize(); i++)
							{
								const float value = connectionState.reliableBytesSentHistory[i];
								min = std::min(min, value);
								max = std::max(max, value);
								average += value;
							}
							average /= float(connectionState.reliableBytesSentHistory.getSize());
							ImGui::PlotLines("Reliable byte send rate",
								connectionState.reliableBytesSentHistory.getBuffer(),
								int(connectionState.reliableBytesSentHistory.getSize()),
								int(connectionState.reliableBytesSentHistory.getBeginOffset()),
								formatString("Reliable acknowledged user bytes sent\nMax: %s/s\nMin: %s/s\nAvg: %s/s",
									toByteString(uint64_t(max / historyRecordInterval.asSeconds())),
									toByteString(uint64_t(min / historyRecordInterval.asSeconds())),
									toByteString(uint64_t(average / historyRecordInterval.asSeconds()))).c_str(),
								min, max, ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().ScrollbarSize, 100.0f));
						}

						// Reliable receive
						{
							const uint64_t reliableReceivedBytesInQueue = connection->getReliableReceivedBytesInQueue();
							ImGui::Text("Reliable received bytes in queue: " + std::to_string(reliableReceivedBytesInQueue));
							const uint64_t reliableStreamOffsetReceive = connection->getReliableStreamOffsetReceive();
							ImGui::Text("Reliable stream offset receive: " + std::to_string(reliableStreamOffsetReceive));
							if (now - connectionState.prevReliableBytesReceived.first >= historyRecordInterval)
							{
								if (connectionState.reliableBytesReceivedHistory.isFull())
								{
									connectionState.reliableBytesReceivedHistory.popFront();
								}
								const uint64_t totalReceivedBytes = reliableStreamOffsetReceive + reliableReceivedBytesInQueue;
								const uint64_t advancement = totalReceivedBytes - connectionState.prevReliableBytesReceived.second;
								connectionState.reliableBytesReceivedHistory.pushBack(float(advancement));
								connectionState.prevReliableBytesReceived.first = now;
								connectionState.prevReliableBytesReceived.second = totalReceivedBytes;
							}
							float min = std::numeric_limits<float>::max();
							float max = 0.0f;
							float average = 0.0f;
							for (size_t i = 0; i < connectionState.reliableBytesReceivedHistory.getSize(); i++)
							{
								const float value = connectionState.reliableBytesReceivedHistory[i];
								min = std::min(min, value);
								max = std::max(max, value);
								average += value;
							}
							average /= float(connectionState.reliableBytesReceivedHistory.getSize());
							ImGui::PlotLines("Reliable byte receive rate",
								connectionState.reliableBytesReceivedHistory.getBuffer(),
								int(connectionState.reliableBytesReceivedHistory.getSize()),
								int(connectionState.reliableBytesReceivedHistory.getBeginOffset()),
								formatString("Reliable user bytes received\nMax: %s/s\nMin: %s/s\nAvg: %s/s",
									toByteString(uint64_t(max / historyRecordInterval.asSeconds())),
									toByteString(uint64_t(min / historyRecordInterval.asSeconds())),
									toByteString(uint64_t(average / historyRecordInterval.asSeconds()))).c_str(),
								min, max, ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().ScrollbarSize, 100.0f));
						}

						// Send quota
						{
							const double sendQuotaPerSecond = connection->getSendQuotaPerSecond();
							const float multiplier = 1024.0f;
							ImGui::Text("Buffered reliable send quota: " + std::to_string(connection->getReliableSendQuota()));
							ImGui::Text("Buffered unreliable send quota: " + std::to_string(connection->getUnreliableSendQuota()));
							ImGui::Text("Send quota per second: " + toByteString(uint64_t(sendQuotaPerSecond)) + "/s");
							if (now - connectionState.prevSendQuotaPerSecondTime >= historyRecordInterval)
							{
								if (connectionState.sendQuotaPerSecondHistory.isFull())
								{
									connectionState.sendQuotaPerSecondHistory.popFront();
								}
								connectionState.sendQuotaPerSecondHistory.pushBack(float(sendQuotaPerSecond) / multiplier);
								connectionState.prevSendQuotaPerSecondTime = now;
							}
							float min = std::numeric_limits<float>::max();
							float max = 0.0f;
							float average = 0.0f;
							for (size_t i = 0; i < connectionState.sendQuotaPerSecondHistory.getSize(); i++)
							{
								const float value = connectionState.sendQuotaPerSecondHistory[i] * multiplier;
								min = std::min(min, value);
								max = std::max(max, value);
								average += value;
							}
							average /= float(connectionState.sendQuotaPerSecondHistory.getSize());
							ImGui::PlotLines("Send quota per second",
								connectionState.sendQuotaPerSecondHistory.getBuffer(),
								int(connectionState.sendQuotaPerSecondHistory.getSize()),
								int(connectionState.sendQuotaPerSecondHistory.getBeginOffset()),
								formatString("Send quota per second\nMax: %s/s\nMin: %s/s\nAvg: %s/s",
									toByteString(uint64_t(max)),
									toByteString(uint64_t(min)),
									toByteString(uint64_t(average))).c_str(),
								min / multiplier, max / multiplier, ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().ScrollbarSize, 100.0f));
						}
					}
				}
			}
			ImGui::End();
		}
	}
}
