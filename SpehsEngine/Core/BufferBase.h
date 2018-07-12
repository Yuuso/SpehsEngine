#pragma once

namespace se
{
	class Handshake;
	/*
		An abstract base class for read/write buffers.
		Buffer read/write handles endianness conversion.
	*/
	class BufferBase
	{
	public:
		friend class Handshake;
	public:
		BufferBase();
		virtual ~BufferBase() = 0;
			
		/* Returns the total capacity of the buffer. */
		virtual size_t getCapacity() const = 0;

		/* Returns the number of bytes written/read using the write/read method. */
		size_t getOffset() const;

	protected:
		size_t offset = 0;
	};
}