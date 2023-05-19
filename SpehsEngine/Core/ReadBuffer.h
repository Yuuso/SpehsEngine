#pragma once

#include "SpehsEngine/Core/BufferBase.h"
#include "SpehsEngine/Core/ByteVector.h"
#include "SpehsEngine/Core/ByteView.h"
#include "SpehsEngine/Core/Endianness.h"
#include "SpehsEngine/Core/SE_Assert.h"


namespace se
{
	/*
		Does NOT own the underlying data!
	*/
	class LegacyReadBuffer : public BufferBase
	{
	public:
		LegacyReadBuffer(const void* pointedMemory, const size_t length);
		~LegacyReadBuffer() override;

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
					se_assert(false);
					return false;
				}

				if (hostByteOrder == networkByteOrder)
				{
					// Read in native byte order
					memcpy((void*)&t, &data[offset], bytes);
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
					se_assert(false);
					return false;
				}
				std::vector<std::byte> vector;
				t.swap(vector);
				vector.resize(bytes);
				memcpy(vector.data(), &data[offset], bytes);
				t.swap(vector);
				offset += bytes;
				return true;
			}
			else if constexpr (IsStaticByteView<T>::value)
			{
				// Static byte view
				constexpr uint32_t bytes = uint32_t(T::getSize());
				if (offset + bytes > size)
				{
					se_assert(false);
					return false;
				}
				memcpy(t.getData(), &data[offset], bytes);
				offset += bytes;
				return true;
			}
			else
			{
				se_assert(false);
				return false;
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
