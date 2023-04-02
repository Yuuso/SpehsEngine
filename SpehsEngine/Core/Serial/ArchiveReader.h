#pragma once

#include "SpehsEngine/Core/Serial/BinaryReader.h"
#include "SpehsEngine/Core/Serial/ArchiveIdStack.h"
#include "SpehsEngine/Core/STLUnorderedMapUtilityFunctions.h"
#include <vector>


namespace se
{
	class ArchiveReader
	{
	public:

		static inline constexpr bool getKeyEnabled() { return true; }
		static inline constexpr bool getWritingEnabled() { return false; }
		static inline constexpr bool getReadingEnabled() { return true; }
		inline uint8_t getVersion() { return version; }

		ArchiveReader(const uint8_t* const _data, const size_t _size);

		template<typename T>
		inline bool serial(T& _value, const std::string_view _key);

	private:

		struct Impl;

		struct Object
		{
			uint32_t index = 0;
			uint32_t size = 0;
		};

		template<typename T>
		uint64_t beginObject(const std::string_view _key) { return archiveIdStack.pushId(ArchiveIdStack::getTypeId<T>(), _key); }
		void endObject() { archiveIdStack.popId(); }

		ArchiveIdStack archiveIdStack;
		std::unordered_map<uint64_t, Object> objects;
		std::vector<uint8_t> objectData;
		uint8_t version = 0;
	};

	template<typename T>
	inline bool ArchiveReader::serial(T& _value, const std::string_view _key)
	{
		if constexpr (std::is_enum<T>::value)
		{
			return serial<std::underlying_type<T>::type>((typename std::underlying_type<T>::type&)_value, _key);
		}
		else if constexpr (!std::is_class<T>::value)
		{
			const uint64_t objectHash = beginObject<T>(_key);
			if (const Object* const object = tryFind(objects, objectHash))
			{
				se_assert(size_t(object->size) == sizeof(T));
				BinaryReader binaryReader(objectData.data() + object->index, size_t(object->size));
				const bool readResult = binaryReader.serial(_value);
				se_assert(readResult);
				(void)readResult;
			}
			endObject();
			return true;
		}
		else if constexpr (std::is_same<T, ByteVector>::value)
		{
			// Read raw data block
			const uint64_t objectHash = beginObject<T>(_key);
			if (const Object* const object = tryFind(objects, objectHash))
			{
				std::vector<std::byte> data;
				_value.swap(data);
				data.clear();
				data.resize(size_t(object->size));
				memcpy(data.data(), objectData.data() + object->index, size_t(object->size));
				_value.swap(data);
			}
			endObject();
			return true;
		}
		else
		{
			// Free reader
			beginObject<T>(_key);
			const bool result = Serial<SerialTag<T>::type>::template serial<ArchiveReader, T&>(*this, _value);
			endObject();
			return result;
		}
	}
}
