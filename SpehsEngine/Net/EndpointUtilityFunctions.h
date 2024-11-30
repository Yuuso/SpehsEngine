#pragma once

#include "SpehsEngine/Net/Endpoint.h"


namespace se
{
	namespace net
	{
		Endpoint commandLineArgumentsToEndpoint(const int argc, const char** argv);
	}
}
