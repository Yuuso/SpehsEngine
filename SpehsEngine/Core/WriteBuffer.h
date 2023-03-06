#pragma once

#include "SpehsEngine/Core/BufferBase.h"
#include "SpehsEngine/Core/Endianness.h"
#include "SpehsEngine/Core/HasMemberFunction.h"
#include "SpehsEngine/Core/ByteView.h"


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
		void setOffset(const size_t offset);
		void resize(const size_t size);
		void reserve(const size_t capacity);
		void clear();

		template<typename T>
		void write (const T& t)
		{
			if constexpr (std::is_enum<T>::value)
			{
				// Enum
				write((typename std::underlying_type<T>::type&)t);
			}
			else if constexpr (!std::is_class<T>::value)
			{
				// Not class
				const size_t bytes = sizeof(T);
				if (offset + bytes > data.size())
				{
					data.resize(offset + bytes);
				}

				if (hostByteOrder == networkByteOrder)
				{
					// Write in native order
					memcpy(&data[offset], &t, bytes);
					offset += bytes;
				}
				else
				{
					// Write in reversed order
					size_t endOffset = bytes;
					for (size_t i = 0; i < bytes; i++)
					{
						data[offset++] = ((const uint8_t*)&t)[--endOffset];
					}
				}
			}
			else if constexpr (std::is_same<T, ByteVector>::value || std::is_same<T, ByteView>::value)
			{
				// Byte vector/view
				const uint32_t vectorSize = uint32_t(t.getSize());
				const size_t bytes = sizeof(vectorSize) + size_t(vectorSize);
				if (offset + bytes > data.size())
				{
					data.resize(offset + bytes);
				}
				write(vectorSize);

				if (hostByteOrder == networkByteOrder)
				{
					// Write in native order
					memcpy(&data[offset], t.getData(), vectorSize);
					offset += vectorSize;
				}
				else
				{
					// Write in reversed order
					size_t endOffset = bytes;
					for (uint32_t i = 0; i < vectorSize; i++)
					{
						data[offset++] = (unsigned char)(*(t.getData() + endOffset));
						--endOffset;
					}
				}
			}
			else if constexpr (has_free_write<T>::value)
			{
				// Free write
				writeToBuffer(*this, t);
			}
			else
			{
				// Class must have member write
				t.write(*this);
			}
		}

		// Writes at specified offset without moving the current offset
		template<typename T>
		void writeAt(const T& t, const size_t _offset)
		{
			const size_t restoreOffset = getOffset();
			setOffset(_offset);
			write(t);
			setOffset(restoreOffset);
		}

		void swap(std::vector<uint8_t>& other) { data.swap(other); offset = data.size(); }

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
