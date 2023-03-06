#include "stdafx.h"
#include "SpehsEngine/Core/Serial/ArchiveReader.h"


namespace se
{
	struct ArchiveReader::Impl
	{
		static inline bool readVersion1(BinaryReader& binaryReader, std::unordered_map<uint64_t, Object>& _objects, std::vector<uint8_t>& _objectData)
		{
			constexpr size_t objectHeaderSize = sizeof(uint64_t) + sizeof(uint32_t);
			uint32_t objectCount = 0;
			se_reader(binaryReader, objectCount, "");
			std::unordered_map<uint64_t, Object> objects;
			uint32_t objectDataIndex = 0;
			uint32_t readObjectCount = 0;
			while (readObjectCount < objectCount)
			{
				uint64_t objectHash = 0;
				uint32_t objectSize = 0;
				se_reader(binaryReader, objectHash, "");
				se_reader(binaryReader, objectSize, "");
				se_assert(!tryFind(objects, objectHash));
				Object& object = objects[objectHash];
				object.index = objectDataIndex;
				object.size = objectSize;
				objectDataIndex += objectSize;
				readObjectCount++;
			}
			se_assert(binaryReader.getBytesRemaining() == objectDataIndex);
			if (binaryReader.getBytesRemaining() != objectDataIndex)
			{
				// Stream is broken
				return false;
			}
			std::vector<uint8_t> objectData;
			objectData.resize(objectDataIndex);
			memcpy(objectData.data(), binaryReader.getData() + binaryReader.getOffset(), objectDataIndex);
			std::swap(objects, _objects);
			std::swap(objectData, _objectData);
			return true;
		}

		static inline bool readImpl(const uint8_t* const _data, const size_t _size,
			std::unordered_map<uint64_t, Object>& _objects, std::vector<uint8_t>& _objectData, uint8_t& _version)
		{
			_version = 0;
			uint8_t version = 0;
			BinaryReader binaryReader(_data, _size);
			se_reader(binaryReader, version, "");
			if (version == 0)
			{
				// Invalid version
				return false;
			}
			else if (version == 1)
			{
				if (readVersion1(binaryReader, _objects, _objectData))
				{
					_version = version;
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// Cannot read future versions / the version value is corrupt
				se::log::error("ArchiveReader failed to read, unhandled version: " + std::to_string(version));
				return false;
			}
		}
	};

	ArchiveReader::ArchiveReader(const uint8_t* const _data, const size_t _size)
	{
		Impl::readImpl(_data, _size, objects, objectData, version);
	}
}
