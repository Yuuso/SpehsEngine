#pragma once

#include "SpehsEngine/Core/Serial/BinaryWriter.h"
#include "SpehsEngine/Core/Serial/ArchiveIdStack.h"


namespace se
{
	class ArchiveWriter
	{
	public:

		static inline constexpr bool getKeyEnabled() { return true; }
		static inline constexpr bool getWritingEnabled() { return true; }
		static inline constexpr bool getReadingEnabled() { return false; }

		template<typename T>
		inline void serial(const T& _value, const std::string_view _key);

		std::vector<uint8_t> generateData() const;

	private:

		template<typename T>
		uint64_t beginObject(const std::string_view _key) { return archiveIdStack.pushId(ArchiveIdStack::getTypeId<T>(), _key); }
		void endObject() { archiveIdStack.popId(); }

		ArchiveIdStack archiveIdStack;
		BinaryWriter binaryWriter1;
		BinaryWriter binaryWriter2;
	};

	template<typename T>
	inline void ArchiveWriter::serial(const T& _value, const std::string_view _key)
	{
		if constexpr (std::is_enum<T>::value)
		{
			serial<std::underlying_type<T>::type>((const typename std::underlying_type<T>::type&)_value, _key);
		}
		else if constexpr (!std::is_class<T>::value)
		{
			const uint64_t objectHash = beginObject<T>(_key);
			const uint32_t objectSize = sizeof(T);
			binaryWriter1.serial(objectHash);
			binaryWriter1.serial(objectSize);
			binaryWriter2.serial(_value);
			endObject();
		}
		else if constexpr (std::is_same<T, ByteView>::value)
		{
			// Write raw data block
			const uint64_t objectHash = beginObject<T>(_key);
			const uint32_t objectSize = uint32_t(_value.getSize());
			binaryWriter1.serial(objectHash);
			binaryWriter1.serial(objectSize);
			std::vector<uint8_t> data;
			binaryWriter2.swap(data);
			const size_t offset = data.size();
			data.resize(data.size() + size_t(objectSize));
			memcpy(data.data() + offset, _value.getData(), _value.getSize());
			binaryWriter2.swap(data);
			endObject();
		}
		else if constexpr (serial_detail::has_free_writer<ArchiveWriter, T>::value)
		{
			// Free writer
			beginObject<T>(_key);
			writer(*this, _value);
			endObject();
		}
		else
		{
			// Must have member writer
			beginObject<T>(_key);
			_value.writer(*this);
			endObject();
		}
	}
}
