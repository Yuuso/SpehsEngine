#pragma once

#include "SpehsEngine/Core/Serial/BinaryWriter.h"
#include "SpehsEngine/Core/Serial/ArchiveIdStack.h"
#include "SpehsEngine/Core/Log.h"


namespace se
{
	class ArchiveWriter
	{
	public:

		static inline constexpr bool getKeyEnabled() { return true; }
		static inline constexpr bool getWritingEnabled() { return true; }
		static inline constexpr bool getReadingEnabled() { return false; }

		template<typename T>
		inline bool serial(const T& _value, const std::string_view _key);

		std::vector<uint8_t> generateData() const;

	private:

		static constexpr size_t valueHeaderSize = sizeof(uint32_t) + sizeof(uint32_t); // Hash + size
		static constexpr size_t containerHeaderSize = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t); // Hash + values begin + values end + child container count

		BinaryWriter valueHeaders; // hash + size
		BinaryWriter valueData;
		BinaryWriter containers; // hash + value count + container count
		uint32_t containerCounter = 0;
	};

	template<typename T>
	inline bool ArchiveWriter::serial(const T& _value, const std::string_view _key)
	{
		if constexpr (std::is_enum<T>::value)
		{
			return serial<std::underlying_type<T>::type>((const typename std::underlying_type<T>::type&)_value, _key);
		}
		else if constexpr (!std::is_class<T>::value)
		{
			const uint32_t hash = getArchiveHash<T>(_key);
			const uint32_t size = sizeof(T);
			valueHeaders.serial(hash);
			valueHeaders.serial(size);
			valueData.serial(_value);
			return true;
		}
		else if constexpr (std::is_same<T, ByteView>::value)
		{
			// Write raw data block
			const uint32_t hash = getArchiveHash<T>(_key);
			const uint32_t size = uint32_t(_value.getSize());
			valueHeaders.serial(hash);
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
			const uint32_t hash = getArchiveHash<T>(_key);
			const uint32_t valuesBegin = uint32_t(valueHeaders.getOffset() / valueHeaderSize);
			uint32_t valuesEnd = 0;
			uint32_t containerCount = 0;
			containers.serial(hash);
			containers.serial(valuesBegin);
			const size_t valuesEndOffset = containers.getOffset();
			containers.serial(valuesEnd);
			const size_t containerCountOffset = containers.getOffset();
			containers.serial(containerCount);
			const uint32_t containerCounterBegin = ++containerCounter;
			const bool result = Serial<SerialTag<T>::type>::template serial<ArchiveWriter, const T&>(*this, _value);
			valuesEnd = uint32_t(valueHeaders.getOffset() / valueHeaderSize);
			containerCount = containerCounter - containerCounterBegin;
			containerCounter = containerCounterBegin;
			containers.writeAt(valuesEnd, valuesEndOffset);
			containers.writeAt(containerCount, containerCountOffset);
			return true;
		}
	}
}
