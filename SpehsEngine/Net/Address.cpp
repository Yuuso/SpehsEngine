#include "stdafx.h"
#include "Address.h"
#include <SpehsEngine/Core/ReadBuffer.h>
#include <SpehsEngine/Core/WriteBuffer.h>

namespace se
{
	namespace net
	{
		const Address Address::invalid = Address(std::string("0.0.0.0"));
	}

	void writeToBuffer(WriteBuffer& buffer, const net::Address& address)
	{
		buffer.write(address.value);
	}

	void readFromBuffer(ReadBuffer& buffer, net::Address& address)
	{
		buffer.read(address.value);
	}
}