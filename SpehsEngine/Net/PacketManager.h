#pragma once


namespace se
{
	namespace net
	{
		template<typename PacketType>
		class PacketManager
		{
		public:

			template<typename Packet>
			void registerReceiveHandler(const PacketType packetType, ScopedConnection& scopedConnection, const std::function<void(Packet&)>& receiveHandler)
			{
				Signal<void(BinaryReader&)>& signal = receiveHandlerSignals[packetType];
				se_assert(signal.isEmpty() && "Multiple sources shouldn't connect as a receive handler to a single packet type at the same time.");
				if (signal.isEmpty())
				{
					scopedConnection = signal.connect([this, receiveHandler](BinaryReader& binaryReader)
						{
							Packet packet;
							if (binaryReader.serial(packet))
							{
								receiveHandler(packet);
							}
							else
							{
								log::error("Failed to read packet contents");
							}
						});
				}
			}

			bool processPacket(const PacketType packetType, BinaryReader& binaryReader)
			{
				typename std::unordered_map<PacketType, Signal<void(BinaryReader&)>>::iterator it = receiveHandlerSignals.find(packetType);
				if (it != receiveHandlerSignals.end() && !it->second.isEmpty())
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
			std::unordered_map<PacketType, Signal<void(BinaryReader&)>> receiveHandlerSignals;
		};
	}
}