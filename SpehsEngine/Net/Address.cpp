#include "stdafx.h"
#include "SpehsEngine/Net/Address.h"


namespace se
{
	namespace net
	{
		const Address Address::invalid = Address(std::string("0.0.0.0"));

		Address::Address()
			: value(invalid.value)
		{

		}
	}
}
