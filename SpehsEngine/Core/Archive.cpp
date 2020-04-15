#include "stdafx.h"
#include "SpehsEngine/Core/Archive.h"

#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Core/STLUnorderedMapUtilityFunctions.h"

namespace se
{
	Archive::Archive()
	{

	}

	void Archive::clear()
	{
		data.clear();
	}

	void Archive::write(WriteBuffer& writeBuffer) const
	{
		writeBuffer.write(uint32_t(data.size()));
		for (std::unordered_map<uint32_t, WriteBuffer>::const_iterator it = data.begin(); it != data.end(); it++)
		{
			writeBuffer.write(it->first);
			writeBuffer.write(uint32_t(it->second.getSize()));
			for (uint32_t i = 0; i < it->second.getSize(); i++)
			{
				writeBuffer.write(*it->second[i]);
			}
		}
	}

	bool Archive::read(ReadBuffer& readBuffer)
	{
		clear();
		uint32_t containerSize;
		se_read(readBuffer, containerSize);
		for (uint32_t c = 0; c < containerSize; c++)
		{
			uint32_t dataHash;
			se_read(readBuffer, dataHash);
			se_assert(data.find(dataHash) == data.end());
			WriteBuffer& writeBuffer = data[dataHash];
			uint32_t dataSize;
			se_read(readBuffer, dataSize);
			for (uint32_t d = 0; d < dataSize; d++)
			{
				unsigned char byte;
				se_read(readBuffer, byte);
				writeBuffer.write(byte);
			}
		}
		return true;
	}

	void Archive::write(const std::string& valueName, const Archive& archive)
	{
		WriteBuffer writeBuffer;
		archive.write(writeBuffer);
		write(valueName, writeBuffer);
	}
	
	bool Archive::read(const std::string& valueName, Archive& archive) const
	{
		WriteBuffer writeBuffer;
		if (!read(valueName, writeBuffer))
		{
			return false;
		}
		if (writeBuffer.getSize() > 0)
		{
			ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
			return archive.read(readBuffer);
		}
		else
		{
			return true;
		}
	}

	void Archive::write(const std::string& valueName, const se::WriteBuffer& writeBuffer)
	{
		const uint32_t hash = getDataHash(typeid(se::WriteBuffer).name(), valueName);
		se_assert(data.find(hash) == data.end());
		data[hash] = writeBuffer;
	}

	bool Archive::read(const std::string& valueName, se::WriteBuffer& writeBuffer) const
	{
		const std::unordered_map<uint32_t, WriteBuffer>::const_iterator it = data.find(getDataHash(typeid(se::WriteBuffer).name(), valueName));
		if (it != data.end())
		{
			writeBuffer = it->second;
			return true;
		}
		else
		{
			return true;
		}
	}

	uint32_t Archive::getDataHash(const std::string& typeName, const std::string& valueName) const
	{
		const uint32_t seed = 1;
		std::vector<uint8_t> hashData(typeName.size() + 1 + valueName.size());
		memcpy(hashData.data(), typeName.data(), typeName.size());
		//Separate type from value by a character that would not belong to a type, so that type-value name pairs like 'a'-'bc' and 'ab'-'c' wont produce the same hash
		hashData[typeName.size()] = 0;
		memcpy(hashData.data() + typeName.size() + 1, valueName.data(), valueName.size());
		return murmurHash3_x86_32(hashData.data(), hashData.size(), seed);
	}
}
