#pragma once

#include "SpehsEngine/Core/DeepCopyUniquePtr.h"
#include "SpehsEngine/Core/UniquePtrUtilityFunctions.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"


namespace se
{
	template<typename T>
	inline void writeToBuffer(WriteBuffer& writeBuffer, const DeepCopyUniquePtr<T>& pointer)
	{
		writeToBuffer(writeBuffer, pointer.getImpl());
	}

	template<typename T>
	inline bool readFromBuffer(ReadBuffer& readBuffer, DeepCopyUniquePtr<T>& pointer)
	{
		return readFromBuffer(readBuffer, pointer.getImpl());
	}

	template<typename T> struct SerialTag<DeepCopyUniquePtr<T>> { typedef SerialTagUniquePtr type; };
}
