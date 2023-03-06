#pragma once

#include "SpehsEngine/Core/BufferBase.h"
#include "SpehsEngine/Core/Endianness.h"
#include "SpehsEngine/Core/HasMemberFunction.h"
#include "SpehsEngine/Core/ByteVector.h"


namespace se
{
	/*
		Does NOT own the underlying data!
	*/
	class ReadBuffer : public BufferBase
	{
	public:
		SPEHS_HAS_MEMBER_FUNCTION(read, has_member_read);
		template<class> struct type_sink { typedef void type; }; // consumes a type, and makes it `void`
		template<class T> using type_sink_t = typename type_sink<T>::type;
		template<class T, class = void> struct has_free_read : std::false_type {};
		template<class T> struct has_free_read<T, type_sink_t<decltype(readFromBuffer(std::declval<ReadBuffer&>(), std::declval<T&>()), bool())>> : std::true_type {};
	public:
		ReadBuffer(const void* pointedMemory, const size_t length);
		~ReadBuffer() override;

		//Is class, has member read
		template<class T>
		inline bool read(T& t)
		{
			if constexpr (std::is_enum<T>::value)
			{
				// Enum
				return read((typename std::underlying_type<T>::type&)t);
			}
			else if constexpr (!std::is_class<T>::value)
			{
				// Not class
				const size_t bytes = sizeof(T);
				if (offset + bytes > size)
				{
					return false;
				}

				if (hostByteOrder == networkByteOrder)
				{
					// Read in native byte order
					memcpy(&t, &data[offset], bytes);
				}
				else
				{
					// Read in reversed byte order
					size_t readOffset = offset + bytes;
					for (size_t i = 0; i < bytes; i++)
					{
						((unsigned char*)&t)[i] = data[--readOffset];
					}
				}

				offset += bytes;
				return true;
			}
			else if constexpr (std::is_same<T, ByteVector>::value)
			{
				// Byte vector
				uint32_t bytes = 0;
				if (!read(bytes))
				{
					return false;
				}
				if (offset + bytes > size)
				{
					return false;
				}

				std::vector<std::byte> vector;
				t.swap(vector);
				vector.resize(bytes);

				if (hostByteOrder == networkByteOrder)
				{
					// Read in native byte order
					memcpy(vector.data(), &data[offset], bytes);
				}
				else
				{
					// Read in reversed byte order
					size_t readOffset = offset + bytes;
					for (size_t i = 0; i < bytes; i++)
					{
						vector[i] = std::byte(data[--readOffset]);
					}
				}
				t.swap(vector);

				offset += bytes;
				return true;
			}
			else if constexpr (has_free_read<T>::value)
			{
				// Free read
				return readFromBuffer(*this, t);
			}
			else
			{
				// Class must have member read
				return t.read(*this);
			}
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
