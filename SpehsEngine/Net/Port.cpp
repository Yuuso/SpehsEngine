#include "Port.h"
#include <SpehsEngine/Core/ReadBuffer.h>
#include <SpehsEngine/Core/WriteBuffer.h>

namespace se
{
	namespace net
	{
		const Port Port::invalid = Port(0);
		const Port Port::default = Port(41623);
	}

	void writeToBuffer(WriteBuffer& buffer, const net::Port& port)
	{
		buffer.write(port.value);
	}

	void readFromBuffer(ReadBuffer& buffer, net::Port& port)
	{
		buffer.read(port.value);
	}
}