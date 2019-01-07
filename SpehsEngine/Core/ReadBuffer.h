#pragma once
#include "BufferBase.h"
#include "Endianness.h"
#include "HasMemberFunction.h"

namespace se
{
	SPEHS_HAS_MEMBER_FUNCTION(read, has_read);
	/*
		Does NOT own the underlying data!
	*/
	class ReadBuffer : public BufferBase
	{
	public:
		ReadBuffer(const void* pointedMemory, const size_t length);
		~ReadBuffer() override;

		//Is class, has mutable read
		template<class T>
		typename std::enable_if<has_read<T, bool(T::*)(ReadBuffer&)>::value, bool>::type read(T& t)
		{
			return t.read(*this);
		}
		//Is class, doesn't have mutable read
		template<class T>
		typename std::enable_if<!has_read<T, bool(T::*)(ReadBuffer&)>::value, bool>::type read(T& t)
		{
			return readFromBuffer(*this, t);
//				se::log::info(typeid(T).name());
//				se_assert(false && "To use 'ReadBuffer::read<T>(T&)' for a class type, the type T must have a 'bool read(ReadBuffer&)' method!");
//#ifdef _WIN32 // NOTE: cannot use static assert because of g++ and SFINAE
//				static_assert(false, "Class type T doesn't have a mutable read method.");
//#endif
		}
		//Isn't class
		template<typename T>
		typename std::enable_if<!std::is_class<T>::value, bool>::type read(T& t)
		{
			const size_t bytes = sizeof(T);
			if (offset + bytes > size)
			{
				return false;
			}

			if (hostByteOrder == networkByteOrder)
			{//Read in native byte order
				memcpy(&t, &data[offset], bytes);
			}
			else
			{//Read in reversed byte order
				size_t readOffset = offset + bytes;
				for (size_t i = 0; i < bytes; i++)
				{
					((unsigned char*)&t)[i] = data[--readOffset];
				}
			}

			offset += bytes;
			return true;
		}

		void translate(const int bytes) override;

		/* Returns readable bytes remaining. */
		size_t getBytesRemaining() const;

		size_t getCapacity() const override { return size; }
		size_t getSize() const override { return size; }

		const unsigned char* operator[](const size_t index) const { return &data[index]; }
			
	private:

		size_t size;
		const unsigned char* data;
	};
}

#define se_read(p_ReadBuffer, p_Value) do { if (!(p_ReadBuffer).read(p_Value)) { return false; } } while (false)
