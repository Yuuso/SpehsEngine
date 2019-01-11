#pragma once
#include <vector>
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"

namespace se
{
	template<typename T, typename SizeType = size_t>
	void writeToBuffer(WriteBuffer& buffer, const std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		se_assert(size_t(std::numeric_limits<SizeType>::max()) >= vector.size() && "Vector size is larger than the maximum of SizeType.");
		const SizeType size = SizeType(vector.size());
		buffer.write(size);
		for (SizeType i = 0; i < size; i++)
		{
			buffer.write(vector[size_t(i)]);
		}
	}

	template<typename T, typename SizeType = size_t>
	bool readFromBuffer(ReadBuffer& buffer, std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size;
		if (!buffer.read(size))
		{
			return false;
		}
		vector.resize(size_t(size));
		for (SizeType i = 0; i < size; i++)
		{
			if (!buffer.read(vector[size_t(i)]))
			{
				return false;
			}
		}
		return true;
	}

	template<typename T, typename SizeType = size_t>
	void writeToArchive(Archive& archive, const std::string& valueName, const std::vector<T>& vector)
	{
		//static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		//Archive subArchive;
		//subArchive.write("size", vector.size());
		//size_t index = 0;
		//for (size_t i = 0; i < vector.size(); i++)
		//{
		//	subArchive.write(std::to_string(i), vector[i]);
		//}
		//archive.write(valueName, subArchive);
		WriteBuffer writeBuffer;
		writeToBuffer(writeBuffer, vector);
		archive.write(valueName, writeBuffer);
	}

	template<typename T, typename SizeType = size_t>
	bool readFromArchive(const Archive& archive, const std::string& valueName, std::vector<T>& vector)
	{
		//static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		//Archive subArchive;
		//archive.read(valueName, subArchive);
		//size_t size = 0;
		//subArchive.read("size", size);
		//vector.resize(size);
		//for (size_t i = 0; i < size; i++)
		//{
		//	if (!subArchive.read(std::to_string(i), vector[i]))
		//	{
		//		return false;
		//	}
		//}
		WriteBuffer writeBuffer;
		if (!archive.read(valueName, writeBuffer))
		{
			return false;
		}
		ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
		readFromBuffer(readBuffer, vector);
		return true;
	}
}
