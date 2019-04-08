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
		SPEHS_HAS_MEMBER_FUNCTION(write, has_member_write);
		template<class> struct type_sink { typedef void type; }; // consumes a type, and makes it `void`
		template<class T> using type_sink_t = typename type_sink<T>::type;
		template<class T, class = void> struct has_free_write : std::false_type {};
		template<class T> struct has_free_write<T, type_sink_t<decltype(writeToBuffer(std::declval<WriteBuffer&>(), std::declval<const T&>()), void())>> : std::true_type {};
	public:
		WriteBuffer();
		~WriteBuffer() override;
		
		void write(se::WriteBuffer& writeBuffer) const;
		bool read(se::ReadBuffer& readBuffer);

		void translate(const int bytes);
		void resize(const size_t size);
		void reserve(const size_t capacity);

		//Const class, has member write
		template<class T>
		typename std::enable_if<has_member_write<T, void(T::*)(WriteBuffer&) const>::value, void>::type write(const T& t)
		{
			t.write(*this);
		}
		//Const class, doesn't have member write but has free write
		template<class T>
		typename std::enable_if<!has_member_write<T, void(T::*)(WriteBuffer&) const>::value && has_free_write<T>::value, void>::type write(const T& t)
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

		bool isEmpty() const { return data.empty(); }
		const uint8_t* getData() const { return data.data(); }
		const uint8_t* operator[](const size_t index) const { return &data[index]; }
		size_t getCapacity() const override { return data.capacity(); }
		size_t getSize() const override { return data.size(); }

	private:

		/* Extends the buffer by increasing its size, without affecting the current offset. */
		bool extend(const size_t addedBytes);

		std::vector<uint8_t> data;
	};
}

/*
	Rationale for this macro:
	Normally we would write 'writeBuffer.write(...)', but then we might accidentally use readBuffer.read(...) in the read function, without handling the read's return value.
	Thus I decided that it would be better to have such macro, and use se_read/write everywhere.
*/
#define se_write(p_WriteBuffer, p_Value) do { (p_WriteBuffer).write(p_Value); } while (false)
