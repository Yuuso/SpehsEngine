#pragma once
#include <vector>
#include "SpehsEngine/Core/Archive.h"
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

	//Class, has write
	template<typename T, typename SizeType = size_t>
	typename std::enable_if<std::is_class<T>::value && Archive::has_write<T, Archive(T::*)() const>::value, Archive>::type
		writeToArchive(const std::vector<T>& vector)
	{
		Archive archive;
		const SizeType size = vector.size();
		se_write_to_archive(archive, size);
		for (size_t i = 0; i < size; i++)
		{
			archive.write(std::to_string(i), vector[i].write());
		}
		WriteBuffer writeBuffer;
		writeToBuffer(writeBuffer, vector);
		se_write_to_archive(archive, writeBuffer);
		return archive;
	}
	template<typename T, typename SizeType = size_t>
	typename std::enable_if<std::is_class<T>::value && Archive::has_write<T, Archive(T::*)() const>::value, Archive>::type
		readFromArchive(const Archive& archive, std::vector<T>& vector)
	{
		SizeType size;
		se_read_from_archive(archive, size);
		vector.resize(size);
		for (size_t i = 0; i < size; i++)
		{
			Archive subArchive;
			if (!archive.read(std::to_string(i), subArchive))
			{
				return false;
			}
			if (!vector[i].read(subArchive))
			{
				return false;
			}
		}
		return true;
	}

	//Class, no write
	template<typename T, typename SizeType = size_t>
	typename std::enable_if<std::is_class<T>::value && !Archive::has_write<T, Archive(T::*)() const>::value, Archive>::type
		writeToArchive(const std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		Archive archive;
		const SizeType size = vector.size();
		se_write_to_archive(archive, size);
		for (size_t i = 0; i < size; i++)
		{
			archive.write(std::to_string(i), writeToArchive(vector[i]));
		}
		WriteBuffer writeBuffer;
		writeToBuffer(writeBuffer, vector);
		se_write_to_archive(archive, writeBuffer);
		return archive;
	}
	template<typename T, typename SizeType = size_t>
	typename std::enable_if<std::is_class<T>::value && !Archive::has_write<T, Archive(T::*)() const>::value, Archive>::type
		readFromArchive(const Archive& archive, std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size;
		se_read_from_archive(archive, size);
		vector.resize(size);
		for (size_t i = 0; i < size; i++)
		{
			Archive subArchive;
			if (!archive.read(std::to_string(i), subArchive))
			{
				return false;
			}
			if (!readFromArchive(vector[i]))
			{
				return false;
			}
		}
		return true;
	}

	//Not class
	template<typename T, typename SizeType = size_t>
	typename std::enable_if<!std::is_class<T>::value, Archive>::type
		writeToArchive(const std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		writeToBuffer<T, SizeType>(writeBuffer, vector);
		Archive archive;
		se_write_to_archive(archive, writeBuffer);
		return archive;
	}
	template<typename T, typename SizeType = size_t>
	typename std::enable_if<!std::is_class<T>::value, bool>::type
		readFromArchive(const Archive& archive, std::vector<T>& vector)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		se_read_from_archive(archive, writeBuffer);
		ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
		return readFromBuffer<T, SizeType>(readBuffer, vector);
	}
}
