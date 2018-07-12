#pragma once
#include "BufferBase.h"
#include "Endianness.h"
#include <vector>
#include "Time.h" // TO BE REMOVED
#include "HasMemberFunction.h"

namespace se
{
	SPEHS_HAS_MEMBER_FUNCTION(write, has_write);
	/*
		Write buffers can only be extended, but never contracted.
		Owns the underlying data.
	*/
	class WriteBuffer : public BufferBase
	{
	public:
		WriteBuffer();
		~WriteBuffer() override;
			
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
//				se::log::info(typeid(T).name());
//				SPEHS_ASSERT(false && "To use 'WriteBuffer::write<T>(const T&)' for a class type, the type T must have a 'void write(WriteBuffer&) const' method!");
//#ifdef _WIN32 // NOTE: cannot use static assert because of g++ and SFINAE
//				static_assert(false, "Class type T doesn't have a const write method.");
//#endif
		}
		//Mutable class, has mutable write
		template<class T>
		typename std::enable_if<has_write<T, void(T::*)(WriteBuffer&)>::value, void>::type write(T& t)
		{
			t.write(*this);
		}
		//Mutable class, has const write
		template<class T>
		typename std::enable_if<has_write<T, void(T::*)(WriteBuffer&) const>::value, void>::type write(T& t)
		{
			t.write(*this);
		}
		//Mutable class, doesn't have mutable or const write
		template<class T>
		typename std::enable_if<!has_write<T, void(T::*)(WriteBuffer&)>::value && !has_write<T, void(T::*)(WriteBuffer&) const>::value, void>::type write(T& t)
		{
			writeToBuffer(*this, t);
//				se::log::info(typeid(T).name());
//				SPEHS_ASSERT(false && "To use 'WriteBuffer::write<T>(T&)' for a class type, the type T must have a 'void write(WriteBuffer&)' or 'void write(WriteBuffer&) const' method!");
//#ifdef _WIN32 // NOTE: cannot use static assert because of g++ and SFINAE
//				static_assert(false, "Class type T doesn't have mutable or const write method.");
//#endif
		}
		//Isn't class
		template<typename T>
		typename std::enable_if<!std::is_class<T>::value, void>::type write (const T& t)
		{
			const size_t bytes = sizeof(T);
			data.resize(data.size() + bytes);

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
					data[offset++] = ((const unsigned char*)&t)[--endOffset];
				}
			}
		}

		const unsigned char* operator[](const size_t index) const { return &data[index]; }
		size_t getCapacity() const override { return data.capacity(); }

	private:

		/* Extends the buffer by increasing its size, without affecting the current offset. */
		bool extend(const size_t addedBytes);

		std::vector<unsigned char> data;
	};
		
	template<typename T>
	void writeToBuffer(WriteBuffer& buffer, const std::vector<T>& vector)
	{
		const size_t size = vector.size();
		buffer.write(size);
		for (size_t i = 0; i < size; i++)
			buffer.write(vector.size());
	}
	void writeToBuffer(WriteBuffer& buffer, const std::string& string);
	void writeToBuffer(WriteBuffer& buffer, const se::time::Time& time);
}