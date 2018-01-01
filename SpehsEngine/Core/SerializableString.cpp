#include <stdint.h>
#include <cstring>
#include "SpehsEngine/Core/SerializableString.h"

namespace spehs
{
	SerializableString::SerializableString()
	{
	}

	SerializableString::SerializableString(const std::string& other) : std::string(other)
	{
	}

	SerializableString& SerializableString::operator=(const std::string& other)
	{
		std::string::operator=(other);
		return *this;
	}

	SerializableString& SerializableString::operator=(const SerializableString& other)
	{
		std::string::operator=(other);
		return *this;
	}

	size_t SerializableString::packetSize() const
	{
		return sizeof(uint16_t) + sizeof(char) * size();
	}

	size_t SerializableString::write(unsigned char* buffer) const
	{
		size_t offset = 0;
		const uint16_t count = size();
		memcpy(&buffer[offset], &count, sizeof(count));								offset += sizeof(count);
		memcpy(&buffer[offset], data(), sizeof(char) * count);						offset += sizeof(char) * count;
		return offset;
	}

	size_t SerializableString::read(const unsigned char* buffer)
	{
		size_t offset = 0;
		uint16_t count;
		memcpy(&count, &buffer[offset], sizeof(count));								offset += sizeof(count);
		resize(count);
		if (count > 0)
		{
			memcpy(&this->operator[](0), &buffer[offset], sizeof(char) * count);	offset += sizeof(char) * count;
		}
		return offset;
	}
}