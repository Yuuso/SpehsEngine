#pragma once

#include "SpehsEngine/Core/StrongInt.h"


namespace se
{
	namespace net
	{
		SE_STRONG_INT(uint32_t, ConnectionId, 0u);
	}
}

SE_STRONG_INT_STD(se::net::ConnectionId);
