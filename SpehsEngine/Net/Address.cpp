#include "stdafx.h"
#include "Address.h"
#include <SpehsEngine/Core/ReadBuffer.h>
#include <SpehsEngine/Core/WriteBuffer.h>

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