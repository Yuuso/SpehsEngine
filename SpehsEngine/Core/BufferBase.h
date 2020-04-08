#pragma once

namespace se
{
	/*
		An abstract base class for read/write buffers.
		Buffer read/write handles endianness conversion.
	*/
	class BufferBase
	{
	public:
		BufferBase();
		virtual ~BufferBase() = 0;
		
		/* Moves buffer write/read pointer by given amount of bytes. */
		virtual void translate(const int bytes) = 0;

		/* Returns the total capacity of the buffer. */
		virtual size_t getCapacity() const = 0;

		/* Returns the current size of the buffer contents. */
		virtual size_t getSize() const = 0;

		/* Returns the number of bytes written/read using the write/read method. */
		size_t getOffset() const;
		
	protected:
		size_t offset = 0;
	};
}