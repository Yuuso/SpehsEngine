#pragma once

#include "SpehsEngine/Core/Serial/BinaryWriter.h"
#include "SpehsEngine/Core/Serial/SerialKey.h"
#include "SpehsEngine/Core/Serial/Serial.h"


namespace se
{
	class ArchiveWriter
	{
	public:

		static inline constexpr bool getKeyEnabled() { return true; }
		static inline constexpr bool getWritingEnabled() { return true; }
		static inline constexpr bool getReadingEnabled() { return false; }

		template<typename T>
		inline bool serial(const T& _value, const SerialKey<T> _key)
		{
			return serialImpl(_value, _key.value);
		}

		std::vector<uint8_t> generateData() const;

	private:

		static constexpr size_t valueHeaderSize = sizeof(uint32_t) + sizeof(uint32_t); // Key + size
		static constexpr size_t containerHeaderSize = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t); // Key + values begin + values end + child container count

		template<typename T>
		inline bool serialImpl(const T& _value, const uint32_t _keyValue);

		BinaryWriter valueHeaders; // key + size
		BinaryWriter valueData;
		BinaryWriter containers; // key + value count + container count
		uint32_t containerCounter = 0;
	};

	template<typename T>
	inline bool ArchiveWriter::serialImpl(const T& _value, const uint32_t _keyValue)
	{
		if constexpr (std::is_enum<T>::value)
		{
			return serialImpl<typename std::underlying_type<T>::type>((const typename std::underlying_type<T>::type&)_value, _keyValue);
		}
		else if constexpr (!std::is_class<T>::value)
		{
			const uint32_t size = sizeof(T);
			valueHeaders.serial(_keyValue);
			valueHeaders.serial(size);
			valueData.serial(_value);
			return true;
		}
		else if constexpr (std::is_same<T, ByteView>::value || std::is_same<T, ByteVector>::value || IsStaticByteView<T>::value || IsConstStaticByteView<T>::value)
		{
			// Write (static or varying size) raw data block
			const uint32_t size = uint32_t(_value.getSize());
			valueHeaders.serial(_keyValue);
			valueHeaders.serial(size);
			std::vector<uint8_t> data;
			valueData.swap(data);
			const size_t offset = data.size();
			data.resize(data.size() + size_t(size));
			memcpy(data.data() + offset, _value.getData(), _value.getSize());
			valueData.swap(data);
			return true;
		}
		else
		{
			// Free writer (container)
			const uint32_t valuesBegin = uint32_t(valueHeaders.getOffset() / valueHeaderSize);
			uint32_t valuesEnd = 0;
			uint32_t containerCount = 0;
			containers.serial(_keyValue);
			containers.serial(valuesBegin);
			const size_t valuesEndOffset = containers.getOffset();
			containers.serial(valuesEnd);
			const size_t containerCountOffset = containers.getOffset();
			containers.serial(containerCount);
			const uint32_t containerCounterBegin = ++containerCounter;
			const bool result = Serial<typename SerialTag<T>::type>::template serial<ArchiveWriter, const T&>(*this, _value);
			(void)result;
			valuesEnd = uint32_t(valueHeaders.getOffset() / valueHeaderSize);
			containerCount = containerCounter - containerCounterBegin;
			containerCounter = containerCounterBegin;
			containers.writeAt(valuesEnd, valuesEndOffset);
			containers.writeAt(containerCount, containerCountOffset);
			return true;
		}
	}
}
