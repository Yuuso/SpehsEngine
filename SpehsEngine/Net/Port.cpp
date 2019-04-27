#include "stdafx.h"
#include "Port.h"
#include <SpehsEngine/Core/ReadBuffer.h>
#include <SpehsEngine/Core/WriteBuffer.h>

namespace se
{
	namespace net
	{
		const Port Port::invalid = Port(0);

		Port::Port()
			: value(invalid.value)
		{

		}
	}

	void writeToBuffer(WriteBuffer& writeBuffer, const net::Port& port)
	{
		writeBuffer.write(port.value);
	}

	bool readFromBuffer(ReadBuffer& readBuffer, net::Port& port)
	{
		se_read(readBuffer, port.value);
		return true;
	}
}
