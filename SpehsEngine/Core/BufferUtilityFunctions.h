#pragma once


namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	/*
		Write and read multiple unsigned 32 bit values to/from buffer.
		Values are written/read with 16 or 8 bits if possible.
		Uses more memory than plain writing/reading if all given values are larger than 16 bit max value.
		Caller is responsible for calling with a same set of pointers for reading what was used for writing.
			For example, writeOptimizedU32(writeBuffer, &w1, &w2, nullptr, nullptr) must be read as readOptimizedU32(readBuffer, &r1, &r2, nullptr, nullptr)
	*/
	void writeOptimizedU32(se::WriteBuffer& writeBuffer, const uint32_t* const value1, const uint32_t* const value2, const uint32_t* const value3, const uint32_t* const value4);
	bool readOptimizedU32(se::ReadBuffer& readBuffer, uint32_t* const value1, uint32_t* const value2, uint32_t* const value3, uint32_t* const value4);
}
