#include "stdafx.h"
#include "SpehsEngine/Debug/ConnectionProfiler.h"

#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Net/ConnectionManager.h"
#include "SpehsEngine/GUI/GUIContext.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Text.h"


namespace se
{
	namespace debug
	{
		ConnectionProfiler::ConnectionProfiler(GUIContext& _guiContext)
			: guiContext(_guiContext)
			, text(*guiContext.getBatchManager().createText())
		{
			if (!text.setFont("Fonts/Anonymous.ttf"/**/, 12))
			{
				log::error("Failed to set font.");
			}
			text.setCameraMatrixState(false);
			text.setColor(Color(1.0f, 1.0f, 1.0f));
			text.setPosition(glm::vec2(5.0f, 5.0f));
		}

		void ConnectionProfiler::addConnection(std::shared_ptr<net::Connection>& _connection)
		{
			if (_connection)
			{
				connections.push_back(_connection);
			}
		}

		void ConnectionProfiler::update()
		{
			SE_SCOPE_PROFILER();
			std::string string;

			for (size_t i = 0; i < connections.size(); i++)
			{
				if (connections[i]->getStatus() == net::Connection::Status::Disconnected)
				{
					connections.erase(connections.begin() + i);
				}
			}

			const int mouseWheelDelta = guiContext.getInputManager().getMouseWheelDelta();
			if (mouseWheelDelta != 0)
			{
				text.setPosition(text.getPosition().x, text.getPosition().y - mouseWheelDelta * (text.getFontHeight()));
			}

			for (const std::shared_ptr<net::Connection>& connection : connections)
			{
				string += "\nPing: " + std::to_string(connection->getPing().asMilliseconds()) + " ms";
				string += "\nMaximum segment size: " + std::to_string(connection->getMaximumSegmentSize());
				string += "\nSend quota per second: " + se::toByteString(uint64_t(connection->getSendQuotaPerSecond()));

				const se::net::Connection::SentBytes sentBytes = connection->getSentBytes();
				string += "\nSent (raw): " + se::toByteString(sentBytes.raw);
				string += "\nSent (reliable): " + se::toByteString(sentBytes.reliable);
				string += "\nSent (reliable resend): " + se::toByteString(sentBytes.reliableResend);
				string += "\nSent (unreliable): " + se::toByteString(sentBytes.unreliable);
				string += "\nSent (acknowledgement): " + se::toByteString(sentBytes.acknowledgement);
				string += "\nSent (path mss discovery): " + se::toByteString(sentBytes.pathMaximumSegmentSizeDiscovery);
				string += "\nReliable unack bytes in queue: " + se::toByteString(connection->getReliableUnacknowledgedBytesInQueue());
				string += "\nReliable ack bytes in queue: " + se::toByteString(connection->getReliableSentAcknowledgedBytesInQueue());
				string += "\nReliable stream offset send: " + std::to_string(connection->getReliableStreamOffsetSend());
				string += "\nReliable stream offset receive: " + std::to_string(connection->getReliableStreamOffsetReceive());

				const se::net::Connection::ReceivedBytes receivedBytes = connection->getReceivedBytes();
				string += "\nReceived (raw): " + se::toByteString(receivedBytes.raw);
				string += "\nReceived (user): " + se::toByteString(receivedBytes.user);
				string += "\nAverage reliable fragment send count: " + std::to_string(connection->getAverageReliableFragmentSendCount());

				string += "\nReliable fragment resend counters: [send count]: % (number of occurences)\n";
				const std::map<uint64_t, uint64_t> reliableFragmentSendCounters = connection->getReliableFragmentSendCounters();
				if (!reliableFragmentSendCounters.empty())
				{
					uint64_t reliableFragmentSendCountersTotalCount = 0u;
					uint64_t reliableFragmentSendCountersMax = 0u;
					for (const std::pair<uint64_t, uint64_t>& pair : reliableFragmentSendCounters)
					{
						reliableFragmentSendCountersTotalCount += pair.second;
						if (pair.second > reliableFragmentSendCountersMax)
						{
							reliableFragmentSendCountersMax = pair.second;
						}
					}
					se_assert(reliableFragmentSendCountersTotalCount > 0);
					for (const std::pair<uint64_t, uint64_t>& pair : reliableFragmentSendCounters)
					{
						const float percentageOfTotal = float(pair.second) / float(reliableFragmentSendCountersTotalCount);
						const float percentageOfMax = float(pair.second) / float(reliableFragmentSendCountersMax);
						const uint64_t dashCount = uint64_t(std::ceil(10.0f * percentageOfMax));
						string.reserve(size_t(uint64_t(string.length()) + dashCount));
						for (uint64_t d = 0; d < dashCount; d++)
						{
							string += "-";
						}
						string += " [" + std::to_string(pair.first) + "]: " + std::to_string(uint64_t(percentageOfTotal * 100.0f)) + "% (" + std::to_string(pair.second) + ")";
						string += "\n";
					}
				}
//
//				{
//					string += "\nTotal time spent locking a mutex:";
//					const net::Connection::MutexTimes mutexLockTimes = connection->getAcquireMutexTimes();
//					std::map<time::Time, std::vector<std::string>> map;
//#define ADD_MUTEX_TIME(p_value) map[mutexLockTimes.p_value].push_back(#p_value); do{} while(false)
//					ADD_MUTEX_TIME(heartbeat);
//					ADD_MUTEX_TIME(estimateRoundTripTime);
//					ADD_MUTEX_TIME(processReceivedPackets);
//					ADD_MUTEX_TIME(deliverOutgoingPackets);
//					ADD_MUTEX_TIME(deliverReceivedPackets);
//					ADD_MUTEX_TIME(spehsReceiveHandler);
//					ADD_MUTEX_TIME(sendPacket);
//					ADD_MUTEX_TIME(sendPacketImpl);
//					ADD_MUTEX_TIME(receivePacket);
//					ADD_MUTEX_TIME(other);
//					for (std::map<time::Time, std::vector<std::string>>::const_reverse_iterator it = map.rbegin(); it != map.rend(); it++)
//					{
//						for (const std::string& valueName : (*it).second)
//						{
//							string += "\n\t" + std::to_string((*it).first.asMilliseconds()) + " ms \t" + valueName;
//						}
//					}
//#undef ADD_MUTEX_TIME
//				}

//				{
//					string += "\nTotal time spent holding a mutex:";
//					const net::Connection::MutexTimes mutexHoldTimes = connection->getHoldMutexTimes();
//					std::map<time::Time, std::vector<std::string>> map;
//#define ADD_MUTEX_TIME(p_value) map[mutexHoldTimes.p_value].push_back(#p_value); do{} while(false)
//					ADD_MUTEX_TIME(heartbeat);
//					ADD_MUTEX_TIME(estimateRoundTripTime);
//					ADD_MUTEX_TIME(processReceivedPackets);
//					ADD_MUTEX_TIME(deliverOutgoingPackets);
//					ADD_MUTEX_TIME(deliverReceivedPackets);
//					ADD_MUTEX_TIME(spehsReceiveHandler);
//					ADD_MUTEX_TIME(sendPacket);
//					ADD_MUTEX_TIME(sendPacketImpl);
//					ADD_MUTEX_TIME(receivePacket);
//					ADD_MUTEX_TIME(other);
//					for (std::map<time::Time, std::vector<std::string>>::const_reverse_iterator it = map.rbegin(); it != map.rend(); it++)
//					{
//						for (const std::string& valueName : (*it).second)
//						{
//							string += "\n\t" + std::to_string((*it).first.asMilliseconds()) + " ms \t" + valueName;
//						}
//					}
//#undef ADD_MUTEX_TIME
//				}
			}

			text.setString(string);
		}

		void ConnectionProfiler::setTextColor(const Color& color)
		{
			text.setColor(color);
		}
	}
}
