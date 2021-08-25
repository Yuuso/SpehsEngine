#pragma once
#include <vector>
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"

namespace se
{
	template<typename T, typename SizeType = uint32_t>
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

	template<typename T, typename SizeType = uint32_t>
	bool readFromBuffer(ReadBuffer& buffer, std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
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

	//Is class
	template<typename T, typename SizeType = uint32_t>
	typename std::enable_if<std::is_class<T>::value, Archive>::type
		writeToArchive(const std::vector<T>& vector)
	{
		se_assert(vector.size() <= size_t(std::numeric_limits<SizeType>::max()));
		Archive archive;
		const SizeType size = SizeType(vector.size());
		archive.write("size", size);
		for (size_t i = 0; i < vector.size(); i++)
		{
			archive.write(std::to_string(i), vector[i]);
		}
		return archive;
	}
	template<typename T, typename SizeType = uint32_t>
	typename std::enable_if<std::is_class<T>::value, bool>::type
		readFromArchive(const Archive& archive, std::vector<T>& vector)
	{
		SizeType size = 0;
		if (!archive.read("size", size))
		{
			return true;
		}
		vector.resize(size_t(size));
		for (size_t i = 0; i < vector.size(); i++)
		{
			if (!archive.read(std::to_string(i), vector[i]))
			{
				return false;
			}
		}
		return true;
	}

	//Not class (Write/ReadBuffer optimization)
	template<typename T, typename SizeType = uint32_t>
	typename std::enable_if<!std::is_class<T>::value, Archive>::type
		writeToArchive(const std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		writeToBuffer<T, SizeType>(writeBuffer, vector);
		Archive archive;
		archive.write("writeBuffer", writeBuffer);
		return archive;
	}
	template<typename T, typename SizeType = uint32_t>
	typename std::enable_if<!std::is_class<T>::value, bool>::type
		readFromArchive(const Archive& archive, std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		if (!archive.read("writeBuffer", writeBuffer))
		{
			return true;
		}
		ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
		return readFromBuffer<T, SizeType>(readBuffer, vector);
	}
}


template<typename T>
T* tryFind(std::vector<T>& vector, const T& t)
{
	for (T& value : vector)
	{
		if (value == t)
		{
			return &value;
		}
	}
	return nullptr;
}
template<typename T>
const T* tryFind(const std::vector<T>& vector, const T& t)
{
	for (const T& value : vector)
	{
		if (value == t)
		{
			return &value;
		}
	}
	return nullptr;
}

template<typename T>
bool findAndErase(std::vector<T>& vector, const T& t)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == t)
		{
			vector.erase(vector.begin() + i);
			return true;
		}
	}
	return false;
}

template<typename T>
bool eraseIndex(std::vector<T>& vector, const size_t index)
{
	if (index < vector.size())
	{
		vector.erase(vector.begin() + index);
		return true;
	}
	return false;
}

template<typename T>
bool findAndSwapOut(std::vector<T>& vector, const T& t)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == t)
		{
			std::swap(vector[i], vector.back());
			vector.pop_back();
			return true;
		}
	}
	return false;
}

template<typename T>
bool swapOutIndex(std::vector<T>& vector, const size_t index)
{
	if (index < vector.size())
	{
		std::swap(vector[index], vector.back());
		vector.pop_back();
		return true;
	}
	return false;
}
