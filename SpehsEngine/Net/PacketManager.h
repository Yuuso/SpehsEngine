#pragma once

#include <functional>
#include "boost/signals2/signal.hpp"


namespace se
{
	namespace net
	{
		template<typename PacketType>
		class PacketManager
		{
		public:

			template<typename Packet>
			void registerReceiveHandler(const PacketType packetType, boost::signals2::scoped_connection& scopedConnection, const std::function<void(Packet&)>& receiveHandler)
			{
				boost::signals2::signal<void(se::BinaryReader&)>& signal = receiveHandlerSignals[packetType];
				se_assert(signal.empty() && "Multiple sources shouldn't connect as a receive handler to a single packet type at the same time.");
				if (signal.empty())
				{
					scopedConnection = signal.connect([this, receiveHandler](se::BinaryReader& binaryReader)
						{
							Packet packet;
							if (binaryReader.serial(packet))
							{
								receiveHandler(packet);
							}
							else
							{
								se::log::error("Failed to read packet contents");
							}
						});
				}
			}

			bool processPacket(const PacketType packetType, se::BinaryReader& binaryReader)
			{
				typename std::unordered_map<PacketType, boost::signals2::signal<void(se::BinaryReader&)>>::iterator it = receiveHandlerSignals.find(packetType);
				if (it != receiveHandlerSignals.end() && !it->second.empty())
				{
					it->second(binaryReader);
					return true;
				}
				else
				{
					return false;
				}
			}

		private:
			std::unordered_map<PacketType, boost::signals2::signal<void(se::BinaryReader&)>> receiveHandlerSignals;
		};
	}
}