#pragma once

#include "SpehsEngine/Core/ByteVector.h"
#include "SpehsEngine/Core/ByteView.h"
#include "SpehsEngine/Core/Endianness.h"
#include "SpehsEngine/Core/Serial/Serial.h"


namespace se
{
	// Writer/reader classes
	class BinaryWriter
	{
	public:

		static inline constexpr bool getKeyEnabled()		{ return false; }
		static inline constexpr bool getWritingEnabled()	{ return true; }
		static inline constexpr bool getReadingEnabled()	{ return false; }

		template<typename T>
		inline bool serial(const T& _value);

		// Writes at specified offset without moving the current offset
		template<typename T>
		void writeAt(const T& _t, const size_t _offset)
		{
			const size_t restoreOffset = getOffset();
			setOffset(_offset);
			serial(_t);
			setOffset(restoreOffset);
		}
		void translate(const int _bytes)
		{
			se_assert(_bytes > 0 || offset >= size_t(std::abs(_bytes)));
			if (offset + _bytes > data.size())
			{
				data.resize(offset + _bytes);
			}
			offset += _bytes;
		}
		inline void setOffset(const size_t _offset)		{ translate(int(_offset) - int(getOffset())); }
		inline const uint8_t* getData() const			{ return data.data(); }
		inline size_t getOffset() const					{ return offset; }
		inline size_t getSize() const					{ return data.size(); }
		inline void reserve(const size_t _capacity)		{ data.reserve(_capacity); }
		inline void resize(const size_t _size)			{ data.resize(_size);	offset = std::min(offset, _size); }
		inline void swap(std::vector<uint8_t>& _other)	{ data.swap(_other);	offset = data.size(); }
		inline void clear()								{ data.clear();			offset = 0; }

	private:
		std::vector<uint8_t> data;
		size_t offset = 0;
	};

	template<typename T>
	inline bool BinaryWriter::serial(const T& _value)
	{
		if constexpr (std::is_enum<T>::value || !std::is_class<T>::value || std::is_same<T, ByteView>::value || std::is_same<T, ByteVector>::value || IsStaticByteView<T>::value || IsConstStaticByteView<T>::value)
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
				if (offset + bytes > data.size())
				{
					data.resize(offset + bytes);
				}

				if (hostByteOrder == networkByteOrder)
				{
					// Write in native order
					memcpy(&data[offset], &_value, bytes);
					offset += bytes;
				}
				else
				{
					// Write in reversed order
					size_t endOffset = bytes;
					for (size_t i = 0; i < bytes; i++)
					{
						data[offset++] = ((const uint8_t*)&_value)[--endOffset];
					}
				}
				return true;
			}
			else if constexpr (std::is_same<T, ByteVector>::value || std::is_same<T, ByteView>::value)
			{
				// Byte vector/view
				const uint32_t vectorSize = uint32_t(_value.getSize());
				const size_t bytes = sizeof(vectorSize) + size_t(vectorSize);
				if (offset + bytes > data.size())
				{
					data.resize(offset + bytes);
				}
				serial(vectorSize);
				memcpy(&data[offset], _value.getData(), vectorSize);
				offset += vectorSize;
				return true;
			}
			else if constexpr (IsStaticByteView<T>::value || IsConstStaticByteView<T>::value)
			{
				// Static byte view
				constexpr size_t bytes = T::getSize();
				if (offset + bytes > data.size())
				{
					data.resize(offset + bytes);
				}
				memcpy(&data[offset], _value.getData(), bytes);
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
			// Free writer
			return Serial<typename SerialTag<T>::type>::template serial<BinaryWriter, const T&>(*this, _value);
		}
	}
}
