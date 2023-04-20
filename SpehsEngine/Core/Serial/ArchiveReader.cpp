#include "stdafx.h"
#include "SpehsEngine/Core/Serial/ArchiveReader.h"
#pragma optimize("", off) // nocommit

namespace se
{
	struct LegacyArchiveReader::Impl
	{
		static inline bool readVersion1(BinaryReader& binaryReader, std::unordered_map<uint64_t, Object>& _objects, std::vector<uint8_t>& _objectData)
		{
			constexpr size_t objectHeaderSize = sizeof(uint64_t) + sizeof(uint32_t);
			uint32_t objectCount = 0;
			se_serial(binaryReader, objectCount, "");
			std::unordered_map<uint64_t, Object> objects;
			uint32_t objectDataIndex = 0;
			uint32_t readObjectCount = 0;
			while (readObjectCount < objectCount)
			{
				uint64_t objectHash = 0;
				uint32_t objectSize = 0;
				se_serial(binaryReader, objectHash, "");
				se_serial(binaryReader, objectSize, "");
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
			se_serial(binaryReader, version, "");
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
				se::log::error("LegacyArchiveReader failed to read, unhandled version: " + std::to_string(version));
				return false;
			}
		}
	};

	LegacyArchiveReader::LegacyArchiveReader(const uint8_t* const _data, const size_t _size)
	{
		Impl::readImpl(_data, _size, objects, objectData, version);
	}

	struct ArchiveReader::Impl
	{
		struct ReadResult
		{
			Container rootContainer;
			std::vector<uint8_t> valueData;
			uint8_t version = 0;
		};

		static inline std::optional<ReadResult> readVersion1(BinaryReader& binaryReader)
		{
			uint32_t valueCount = 0;
			if (!binaryReader.serial(valueCount))
			{
				return std::nullopt;
			}
			uint32_t dataSize = 0;
			if (!binaryReader.serial(dataSize))
			{
				return std::nullopt;
			}

			// Value headers
			std::vector<Value> values;
			std::vector<uint32_t> valueHashes;
			values.resize(size_t(valueCount));
			valueHashes.resize(size_t(valueCount));
			uint32_t valueDataOffset = 0;
			for (uint32_t i = 0; i < valueCount; i++)
			{
				Value& value = values[i];
				if (!binaryReader.serial(valueHashes[i]))
				{
					return std::nullopt;
				}
				if (!binaryReader.serial(value.size))
				{
					return std::nullopt;
				}
				value.index = valueDataOffset;
				valueDataOffset += value.size;
			}

			// Value data
			// NOTE: MSVC seems to have a bug here so the following does not work
			// if (dataSize > binaryReader.getBytesRemaining())
			{
				const size_t bytesRemaining = binaryReader.getBytesRemaining();
				if (dataSize > bytesRemaining)
				{
					return std::nullopt;
				}
			}
			std::vector<uint8_t> valueData;
			valueData.resize(dataSize);
			memcpy(valueData.data(), binaryReader.getData() + binaryReader.getOffset(), dataSize);
			binaryReader.translate(int(dataSize));

			// Containers
			constexpr size_t containerHeaderSize = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
			const size_t containerBytes = binaryReader.getBytesRemaining();
			const uint32_t containerCount = uint32_t(containerBytes / containerHeaderSize);
			if (containerCount * containerHeaderSize != containerBytes)
			{
				se_assert(false);
				return std::nullopt;
			}
			struct ContainerHeader
			{
				uint32_t hash = 0;
				uint32_t valuesEnd = 0;
				uint32_t containersEnd = 0;
			};
			uint32_t valuesEnd = 0;
			uint32_t containersEnd = 0;
			std::vector<ContainerHeader> containerHeaders;
			containerHeaders.resize(1 + containerCount);
			containerHeaders[0].hash = 0;
			containerHeaders[0].valuesEnd = valueCount;
			containerHeaders[0].containersEnd = containerCount;
			for (uint32_t i = 0; i < containerCount; i++)
			{
				ContainerHeader& containerHeader = containerHeaders[i + 1];
				if (!binaryReader.serial(containerHeader.hash) || !binaryReader.serial(containerHeader.valuesEnd) || !binaryReader.serial(containerHeader.containersEnd))
				{
					se_assert(false);
					return std::nullopt;
				}
				if (containerHeader.valuesEnd < valuesEnd || valuesEnd > uint32_t(values.size()))
				{
					se_assert(false);
					return std::nullopt;
				}
				if (containerHeader.containersEnd < containersEnd || containersEnd > containerCount)
				{
					se_assert(false);
					return std::nullopt;
				}
			}
			if (binaryReader.getBytesRemaining() != 0)
			{
				se_assert(false);
				return std::nullopt;
			}

			Container rootContainer;


			//struct Frame
			//{
			//	Container* container = nullptr;
			//	uint32_t valuesEnd = 0;
			//	uint32_t containersEnd = 0;
			//};
			//std::stack<Frame> frameStack;
			//std::vector<Frame> frameVector;
			//frameStack.push(Frame());
			//frameStack.top().container = &rootContainer;
			//frameStack.top().valuesEnd = uint32_t(values.size());
			//frameVector.push_back(frameStack.top());
			//size_t valueIndex = 0;
			//auto progressValues = [&](const size_t end)
			//{
			//	if (frameStack.top().valuesBegin == frameStack.top().valuesEnd)
			//	{
			//		return;
			//	}
			//	while (valueIndex < end)
			//	{
			//		if (valueIndex < values.size())
			//		{
			//			const uint32_t hash = valueHashes[valueIndex];
			//			frameStack.top().container->values[hash] = values[valueIndex];
			//			valueIndex++;
			//		}
			//		else
			//		{
			//			break;
			//		}
			//	}
			//};
			//for (uint32_t i = 0; i < containerCount; i++)
			//{
			//	uint32_t hash = 0;
			//	uint32_t valuesBegin = 0;
			//	uint32_t valuesEnd = 0;
			//	if (!binaryReader.serial(hash) || !binaryReader.serial(valuesBegin) || !binaryReader.serial(valuesEnd))
			//	{
			//		se_assert(false);
			//		return std::nullopt;
			//	}
			//	if (valuesBegin > valuesEnd || valuesEnd > uint32_t(values.size()))
			//	{
			//		se_assert(false);
			//		return std::nullopt;
			//	}
			//	//progressValues(valuesBegin);
			//	while (valuesBegin >= frameStack.top().valuesEnd)
			//	{
			//		se_assert(frameStack.size() > 1);
			//		frameStack.pop();
			//	}
			//	Container& child = frameStack.top().container->containers[hash];
			//	child.containers.reserve(1000); // nocommit TODO
			//	frameStack.push(Frame());
			//	frameStack.top().container = &child;
			//	frameStack.top().valuesBegin = valuesBegin;
			//	frameStack.top().valuesEnd = valuesEnd;
			//	frameVector.push_back(frameStack.top());
			//}
			//if (frameStack.size() == 2)
			//{
			//	frameStack.pop();
			//}
			//progressValues(values.size());
			//se_assert(valueIndex == values.size());
			/*
				NOCOMMIT NOTES TO SELF
					* Values can have size 0 because data buffers can be empty
					* valuesBegin and valuesEnd specify a range of values within the container, but some of those values can be within a sub container
					* The above Frame::container pointers are danging, see the above reserve call
					* The whole container value begin/end indexing idea does not work for retaining the hierarchy...? in a case like empty-empty-empty-value
			*/
			//for (Frame &frame : frameVector)
			//{
			//	if (frame.valuesBegin < frame.valuesEnd)
			//	{
			//		for (size_t v = frame.valuesBegin; v < frame.valuesEnd; v++)
			//		{
			//			const uint32_t hash = valueHashes[v];
			//			frame.container->values[hash] = values[v];
			//		}
			//	}
			//}

			// Result
			ReadResult result;
			result.version = 1;
			result.valueData.swap(valueData);
			std::swap(result.rootContainer, rootContainer);
			return result;
		}

		static inline std::optional<ReadResult> readImpl(const uint8_t* const _data, const size_t _size)
		{
			uint8_t version = 0;
			BinaryReader binaryReader(_data, _size);
			if (!binaryReader.serial(version))
			{
				return std::nullopt;
			}
			if (version == 0)
			{
				// Invalid version
				return std::nullopt;
			}
			else if (version == 1)
			{
				if (std::optional<ReadResult> result = readVersion1(binaryReader))
				{
					return result;
				}
				else
				{
					return std::nullopt;
				}
			}
			else
			{
				// Cannot read future versions / the version value is corrupt
				se::log::error("ArchiveReader failed to read, unhandled version: " + std::to_string(version));
				return std::nullopt;
			}
		}
	};

	ArchiveReader::ArchiveReader(const uint8_t* const _data, const size_t _size)
	{
		if (std::optional<Impl::ReadResult> result = Impl::readImpl(_data, _size))
		{
			std::swap(rootContainer, result->rootContainer);
			std::swap(valueData, result->valueData);
			version = result->version;
		}
		else
		{
			version = 0;
		}
	}
}
