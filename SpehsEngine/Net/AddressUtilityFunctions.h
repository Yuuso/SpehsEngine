#pragma once

#include "SpehsEngine/Net/Address.h"


namespace se
{
	namespace net
	{
		Address getLocalAddress();
		bool isValidEmailAddress(const std::string_view string);
	}
}
