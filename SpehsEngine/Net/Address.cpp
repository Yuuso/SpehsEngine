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

	void writeToBuffer(WriteBuffer& writeBuffer, const net::Address& address)
	{
		writeBuffer.write(address.value);
	}

	bool readFromBuffer(ReadBuffer& readBuffer, net::Address& address)
	{
		se_read(readBuffer, address.value);
		return true;
	}
}
