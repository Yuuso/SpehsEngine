#pragma once

#include "SpehsEngine/Core/BufferBase.h"
#include "SpehsEngine/Core/Endianness.h"
#include "SpehsEngine/Core/ByteView.h"


namespace se
{
	class ReadBuffer;
	/*
		Write buffers can only be extended, but never contracted.
		Owns the underlying data.
	*/
	class LegacyWriteBuffer : public BufferBase
	{
	public:
		
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
				memcpy(&data[offset], t.getData(), vectorSize);
				offset += vectorSize;
			}
			else if constexpr (IsStaticByteView<T>::value || IsConstStaticByteView<T>::value)
			{
				// Static byte view
				constexpr size_t bytes = T::getSize();
				if (offset + bytes > data.size())
				{
					data.resize(offset + bytes);
				}
				memcpy(&data[offset], t.getData(), bytes);
				offset += bytes;
			}
			else
			{
				se_assert(false);
				return false;
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
