#pragma once


namespace se
{
	namespace net
	{
		struct ConnectionSimulationSettings
		{
			float chanceToDropIncoming = 0.0f;
			float chanceToDropOutgoing = 0.0f;
			float chanceToReorderIncoming = 0.0f;
			float chanceToReorderOutgoing = 0.0f;
			time::Time delayIncoming;
			time::Time delayOutgoing;

			// TO BE REMOVED
			float chanceToReorderReceivedPacket = 0.0f;
			uint16_t maximumSegmentSizeIncoming = uint16_t(65535);
			uint16_t maximumSegmentSizeOutgoing = uint16_t(65535);
		};
	}
}
