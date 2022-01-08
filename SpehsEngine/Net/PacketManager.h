#pragma once

#include <functional>


namespace se
{
	namespace net
	{
		template<typename PacketType>
		class PacketManager
		{
		public:

			template<typename Packet>
			void registerReceiveHandler(const PacketType packetType, const std::function<void(Packet&)>& receiveHandler)
			{
				const bool exists = receiveHandlers.find(packetType) != receiveHandlers.end();
				se_assert(!exists);
				if (!exists)
				{
					receiveHandlers[packetType] = [this, receiveHandler](se::ReadBuffer& readBuffer)
					{
						Packet packet;
						if (readBuffer.read(packet))
						{
							receiveHandler(packet);
						}
						else
						{
							se::log::error("Failed to read packet contents");
						}
					};
				}
			}

			bool processPacket(const PacketType packetType, se::ReadBuffer& readBuffer)
			{
				std::unordered_map<PacketType, std::function<void(se::ReadBuffer&)>>::iterator it = receiveHandlers.find(packetType);
				if (it != receiveHandlers.end())
				{
					it->second(readBuffer);
					return true;
				}
				else
				{
					return false;
				}
			}

		private:
			std::unordered_map<PacketType, std::function<void(se::ReadBuffer&)>> receiveHandlers;
		};
	}
}