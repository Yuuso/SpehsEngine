#pragma once

#include "SpehsEngine/Core/WriteBuffer.h"
#include <vector>


namespace se
{
	// Writer/reader classes
	class BinaryWriter
	{
	public:

		static inline constexpr bool getKeyEnabled() { return false; }
		static inline constexpr bool getWritingEnabled() { return true; }
		static inline constexpr bool getReadingEnabled() { return false; }

		template<typename T>
		inline void serial(const T& _value);
		// nocommit what dis?
		template<typename T>
		void writeAt(const T& t, const size_t _offset) { writeBuffer.writeAt(t, _offset); }

		const uint8_t* getData() const { return writeBuffer.getData(); }
		size_t getOffset() const { return writeBuffer.getOffset(); }
		size_t getSize() const { return writeBuffer.getSize(); }
		void reserve(const size_t capacity) { writeBuffer.reserve(capacity); }
		void swap(std::vector<uint8_t>& other) { writeBuffer.swap(other); }
	private:
		WriteBuffer writeBuffer;
	};

	template<typename T>
	inline void BinaryWriter::serial(const T& _value)
	{
		if constexpr (std::is_enum<T>::value || !std::is_class<T>::value || std::is_same<T, ByteView>::value)
		{
			// Built-in
			writeBuffer.write(_value);
		}
		else if constexpr (serial_detail::has_free_writer<BinaryWriter, T>::value)
		{
			// Free writer
			writer(*this, _value);
		}
		else
		{
			// Class method
			_value.writer(*this);
		}
	}
}
