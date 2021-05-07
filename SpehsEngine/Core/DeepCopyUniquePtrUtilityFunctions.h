#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/DeepCopyUniquePtr.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"


namespace se
{
	template<typename T>
	void writeToBuffer(WriteBuffer& writeBuffer, const DeepCopyUniquePtr<T>& pointer)
	{
		const bool exists = pointer.get() != nullptr;
		se_write(writeBuffer, exists);
		if (exists)
		{
			const T& t = *pointer;
			se_write(writeBuffer, t);
		}
	}

	template<typename T>
	bool readFromBuffer(ReadBuffer& readBuffer, DeepCopyUniquePtr<T>& pointer)
	{
		bool exists = false;
		se_read(readBuffer, exists);
		if (exists)
		{
			pointer.reset(new T());
			T& t = *pointer;
			se_read(readBuffer, t);
		}
		else
		{
			pointer.reset();
		}
		return true;
	}

	template<typename T>
	Archive writeToArchive(const DeepCopyUniquePtr<T>& pointer)
	{
		Archive archive;
		const bool exists = pointer.get() != nullptr;
		se_write_to_archive(archive, exists);
		if (exists)
		{
			const T& t = *pointer;
			se_write_to_archive(archive, t);
		}
		return archive;
	}

	template<typename T>
	bool readFromArchive(const Archive& archive, DeepCopyUniquePtr<T>& pointer)
	{
		bool exists = false;
		se_read_from_archive(archive, exists);
		if (exists)
		{
			pointer.reset(new T());
			T& t = *pointer;
			se_read_from_archive(archive, t);
		}
		else
		{
			pointer.reset();
		}
		return true;
	}
}
