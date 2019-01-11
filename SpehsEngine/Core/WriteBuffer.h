#pragma once
#include <vector>
#include "BufferBase.h"
#include "Endianness.h"
#include "HasMemberFunction.h"

namespace se
{
	class ReadBuffer;
	/*
		Write buffers can only be extended, but never contracted.
		Owns the underlying data.
	*/
	class WriteBuffer : public BufferBase
	{
	public:
		SPEHS_HAS_MEMBER_FUNCTION(write, has_write);
	public:
		WriteBuffer();
		~WriteBuffer() override;
		
		void write(se::WriteBuffer& writeBuffer) const;
		bool read(se::ReadBuffer& readBuffer);

		void translate(const int bytes);
		void resize(const size_t size);
		void reserve(const size_t capacity);

		//Const class, has const write
		template<class T>
		typename std::enable_if<has_write<T, void(T::*)(WriteBuffer&) const>::value, void>::type write(const T& t)
		{
			t.write(*this);
		}
		//Const class, doesn't have const write
		template<class T>
		typename std::enable_if<!has_write<T, void(T::*)(WriteBuffer&) const>::value, void>::type write(const T& t)
		{
			writeToBuffer(*this, t);
		}
		//Isn't class
		template<typename T>
		typename std::enable_if<!std::is_class<T>::value, void>::type write (const T& t)
		{
			const size_t bytes = sizeof(T);
			if (offset + bytes > data.size())
			{
				data.resize(offset + bytes);
			}

			if (hostByteOrder == networkByteOrder)
			{//Write in native order
				memcpy(&data[offset], &t, bytes);
				offset += bytes;
			}
			else
			{//Write in reversed order
				size_t endOffset = bytes;
				for (size_t i = 0; i < bytes; i++)
				{
					data[offset++] = ((const uint8_t*)&t)[--endOffset];
				}
			}
		}

		void swap(std::vector<uint8_t>& other) { data.swap(other); }

		const uint8_t* operator[](const size_t index) const { return &data[index]; }
		size_t getCapacity() const override { return data.capacity(); }
		size_t getSize() const override { return data.size(); }

	private:

		/* Extends the buffer by increasing its size, without affecting the current offset. */
		bool extend(const size_t addedBytes);

		std::vector<uint8_t> data;
	};
}
