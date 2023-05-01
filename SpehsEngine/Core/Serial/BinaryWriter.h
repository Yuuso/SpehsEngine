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
		inline bool serial(const T& _value);
		
		template<typename T>
		void writeAt(const T& t, const size_t _offset) { writeBuffer.writeAt(t, _offset); }

		const uint8_t* getData() const { return writeBuffer.getData(); }
		size_t getOffset() const { return writeBuffer.getOffset(); }
		size_t getSize() const { return writeBuffer.getSize(); }
		void reserve(const size_t _capacity) { writeBuffer.reserve(_capacity); }
		void swap(std::vector<uint8_t>& _other) { writeBuffer.swap(_other); }
		void swap(WriteBuffer& _writeBuffer) { std::swap(_writeBuffer, writeBuffer); }
	private:
		WriteBuffer writeBuffer;
	};

	template<typename T>
	inline bool BinaryWriter::serial(const T& _value)
	{
		if constexpr (std::is_enum<T>::value || !std::is_class<T>::value || std::is_same<T, ByteView>::value || std::is_same<T, ByteVector>::value || IsStaticByteView<T>::value || IsConstStaticByteView<T>::value)
		{
			// Built-in
			writeBuffer.write(_value);
			return true;
		}
		else
		{
			// Free writer
			return Serial<SerialTag<T>::type>::template serial<BinaryWriter, const T&>(*this, _value);
		}
	}
}
