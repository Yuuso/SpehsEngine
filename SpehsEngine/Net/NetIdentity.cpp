#include "stdafx.h"
#include "SpehsEngine/Net/NetIdentity.h"

#include "SpehsEngine/Core/RNG.h"


namespace se
{
	namespace net
	{
		NetIdentity getMyNetIdentity()
		{
			static const NetIdentity myNetIdentity = NetIdentity(rng::random<uint64_t>());
			return myNetIdentity;
		}

		bool NetIdentity::fromString(const std::string_view string)
		{
			uint64_t v = 0;
			if (se::fromString(string, v))
			{
				value = v;
				return true;
			}
			else
			{
				return false;
			}
		}

		std::string NetIdentity::toString() const
		{
			return std::to_string(value);
		}
	}
}