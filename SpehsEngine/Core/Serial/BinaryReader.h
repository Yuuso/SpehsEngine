#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/ByteView.h"
#include "SpehsEngine/Core/ByteVector.h"
#include "SpehsEngine/Core/Endianness.h"
#include "SpehsEngine/Core/SE_Assert.h"


namespace se
{
	class BinaryReader
	{
	public:

		static inline constexpr bool getKeyEnabled() { return false; }
		static inline constexpr bool getWritingEnabled() { return false; }
		static inline constexpr bool getReadingEnabled() { return true; }

		BinaryReader(const uint8_t* const _data, const size_t _size)
			: size(_size)
			, data(_data)
		{
		}

		template<typename T>
		inline bool serial(T& _value);

		inline void translate(const int bytes)	{ offset += bytes; }
		inline size_t getBytesRemaining() const	{ return size - offset; }
		inline size_t getSize() const			{ return size; }
		inline size_t getOffset() const			{ return offset; }
		inline const uint8_t* getData() const	{ return data; }

	private:
		const unsigned char* data;
		size_t size;
		size_t offset = 0;
	};

	template<typename T>
	inline bool BinaryReader::serial(T& _value)
	{
		if constexpr (std::is_enum<T>::value || !std::is_class<T>::value || std::is_same<T, ByteVector>::value || IsStaticByteView<T>::value)
		{
			// Built-in
			if constexpr (std::is_enum<T>::value)
			{
				// Enum
				return serial((typename std::underlying_type<T>::type&)_value);
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
					memcpy((void*)&_value, &data[offset], bytes);
				}
				else
				{
					// Read in reversed byte order
					size_t readOffset = offset + bytes;
					for (size_t i = 0; i < bytes; i++)
					{
						((unsigned char*)&_value)[i] = data[--readOffset];
					}
				}

				offset += bytes;
				return true;
			}
			else if constexpr (std::is_same<T, ByteVector>::value)
			{
				// Byte vector
				uint32_t bytes = 0;
				if (!serial(bytes))
				{
					return false;
				}
				if (offset + bytes > size)
				{
					se_assert(false);
					return false;
				}
				std::vector<std::byte> vector;
				_value.swap(vector);
				vector.resize(bytes);
				memcpy(vector.data(), &data[offset], bytes);
				_value.swap(vector);
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
				memcpy(_value.getData(), &data[offset], bytes);
				offset += bytes;
				return true;
			}
			else
			{
				se_assert(false);
				return false;
			}
		}
		else
		{
			// Free reader
			return Serial<typename SerialTag<T>::type>::template serial<BinaryReader, T&>(*this, _value);
		}
	}
}
