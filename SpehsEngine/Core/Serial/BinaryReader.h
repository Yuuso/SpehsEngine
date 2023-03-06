#pragma once

#include "SpehsEngine/Core/ReadBuffer.h"


namespace se
{
	class BinaryReader
	{
	public:

		static inline constexpr bool getKeyEnabled() { return false; }
		static inline constexpr bool getWritingEnabled() { return false; }
		static inline constexpr bool getReadingEnabled() { return true; }

		BinaryReader(const uint8_t* const _data, const size_t _size)
			: readBuffer(_data, _size)
		{
		}

		template<typename T>
		inline bool serial(T& _value);

		inline void translate(const int _bytes) { readBuffer.translate(_bytes); }
		inline size_t getBytesRemaining() const { return readBuffer.getBytesRemaining(); }
		inline size_t getSize() const { return readBuffer.getSize(); }
		inline size_t getOffset() const { return readBuffer.getOffset(); }
		inline const uint8_t* getData() const { return readBuffer[0]; }

	private:
		ReadBuffer readBuffer;
	};

	template<typename T>
	inline bool BinaryReader::serial(T& _value)
	{
		if constexpr (std::is_enum<T>::value || !std::is_class<T>::value || std::is_same<T, ByteVector>::value)
		{
			// Built-in
			return readBuffer.read(_value);
		}
		else if constexpr (serial_detail::has_free_reader<BinaryReader, T>::value)
		{
			// Free reader
			return reader(*this, _value);
		}
		else
		{
			// Must have member reader
			return _value.reader(*this);
		}
	}
}
