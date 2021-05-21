#include "stdafx.h"
#include "SpehsEngine/Core/BufferUtilityFunctions.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"


namespace se
{
	void writeOptimizedU32(se::WriteBuffer& writeBuffer, const uint32_t* const value1, const uint32_t* const value2, const uint32_t* const value3, const uint32_t* const value4)
	{
		const size_t flagsOffset = writeBuffer.getOffset();
		uint8_t flags = 0;
		writeBuffer.write(flags);

		if (value1)
		{
			if (*value1 <= size_t(std::numeric_limits<uint8_t>::max()))
			{
				flags |= 1 << 0;
				writeBuffer.write(uint8_t(*value1));
			}
			else if (*value1 <= size_t(std::numeric_limits<uint16_t>::max()))
			{
				flags |= 1 << 1;
				writeBuffer.write(uint16_t(*value1));
			}
			else
			{
				writeBuffer.write(*value1);
			}
		}

		if (value2)
		{
			if (*value2 <= size_t(std::numeric_limits<uint8_t>::max()))
			{
				flags |= 1 << 2;
				writeBuffer.write(uint8_t(*value2));
			}
			else if (*value2 <= size_t(std::numeric_limits<uint16_t>::max()))
			{
				flags |= 1 << 3;
				writeBuffer.write(uint16_t(*value2));
			}
			else
			{
				writeBuffer.write(*value2);
			}
		}

		if (value3)
		{
			if (*value3 <= size_t(std::numeric_limits<uint8_t>::max()))
			{
				flags |= 1 << 4;
				writeBuffer.write(uint8_t(*value3));
			}
			else if (*value3 <= size_t(std::numeric_limits<uint16_t>::max()))
			{
				flags |= 1 << 5;
				writeBuffer.write(uint16_t(*value3));
			}
			else
			{
				writeBuffer.write(*value3);
			}
		}

		if (value4)
		{
			if (*value4 <= size_t(std::numeric_limits<uint8_t>::max()))
			{
				flags |= 1 << 6;
				writeBuffer.write(uint8_t(*value4));
			}
			else if (*value4 <= size_t(std::numeric_limits<uint16_t>::max()))
			{
				flags |= 1 << 7;
				writeBuffer.write(uint16_t(*value4));
			}
			else
			{
				writeBuffer.write(*value4);
			}
		}

		writeBuffer.writeAt(flags, flagsOffset); // Patch flags
	}

	// NOTE: given pointers must match what was given to writeOptimizedU32
	bool readOptimizedU32(se::ReadBuffer& readBuffer, uint32_t* const value1, uint32_t* const value2, uint32_t* const value3, uint32_t* const value4)
	{
		uint8_t flags = 0;
		se_read(readBuffer, flags);

		if (value1)
		{
			if ((flags & (1 << 0)) != 0)
			{
				uint8_t value = 0;
				se_read(readBuffer, value);
				*value1 = uint32_t(value);
			}
			else if ((flags & (1 << 1)) != 0)
			{
				uint16_t value = 0;
				se_read(readBuffer, value);
				*value1 = uint32_t(value);
			}
			else
			{
				uint32_t value = 0;
				se_read(readBuffer, value);
				*value1 = uint32_t(value);
			}
		}

		if (value2)
		{
			if ((flags & (1 << 2)) != 0)
			{
				uint8_t value = 0;
				se_read(readBuffer, value);
				*value2 = uint32_t(value);
			}
			else if ((flags & (1 << 3)) != 0)
			{
				uint16_t value = 0;
				se_read(readBuffer, value);
				*value2 = uint32_t(value);
			}
			else
			{
				uint32_t value = 0;
				se_read(readBuffer, value);
				*value2 = uint32_t(value);
			}
		}

		if (value3)
		{
			if ((flags & (1 << 4)) != 0)
			{
				uint8_t value = 0;
				se_read(readBuffer, value);
				*value3 = uint32_t(value);
			}
			else if ((flags & (1 << 5)) != 0)
			{
				uint16_t value = 0;
				se_read(readBuffer, value);
				*value3 = uint32_t(value);
			}
			else
			{
				uint32_t value = 0;
				se_read(readBuffer, value);
				*value3 = uint32_t(value);
			}
		}

		if (value4)
		{
			if ((flags & (1 << 6)) != 0)
			{
				uint8_t value = 0;
				se_read(readBuffer, value);
				*value4 = uint32_t(value);
			}
			else if ((flags & (1 << 7)) != 0)
			{
				uint16_t value = 0;
				se_read(readBuffer, value);
				*value4 = uint32_t(value);
			}
			else
			{
				uint32_t value = 0;
				se_read(readBuffer, value);
				*value4 = uint32_t(value);
			}
		}

		return true;
	}
}
