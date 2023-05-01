#include "stdafx.h"
#include "SpehsEngine/Core/Serial/ArchiveWriter.h"


namespace se
{
	std::vector<uint8_t> ArchiveWriter::generateData() const
	{
		uint8_t version = 1; // Current version
		const uint32_t valueCount = uint32_t(valueHeaders.getSize() / valueHeaderSize);
		se_assert((valueCount * valueHeaderSize) == valueHeaders.getSize());
		const uint32_t buffersSize = uint32_t(valueHeaders.getSize() + valueData.getSize() + containers.getSize());
		const uint32_t dataSize = uint32_t(valueData.getSize());
		const size_t headerSize = sizeof(version) + sizeof(valueCount) + sizeof(dataSize);
		const size_t totalSize = headerSize + buffersSize;

		// Single reserve call to cover the entire write
		std::vector<uint8_t> data;
		data.reserve(totalSize);

		{
			BinaryWriter binaryWriter;
			binaryWriter.swap(data);
			binaryWriter.serial(version);
			binaryWriter.serial(valueCount);
			binaryWriter.serial(dataSize);
			binaryWriter.swap(data);
		}

		se_assert(data.size() == headerSize);
		size_t offset = data.size();
		data.resize(totalSize);
		memcpy(data.data() + offset, valueHeaders.getData(),	valueHeaders.getSize());	offset += valueHeaders.getSize();
		memcpy(data.data() + offset, valueData.getData(),		valueData.getSize());		offset += valueData.getSize();
		memcpy(data.data() + offset, containers.getData(),		containers.getSize());		offset += containers.getSize();
		se_assert(offset == data.size());

		//se::log::info("Archive data generated: " + std::to_string(totalSize) + " B"
		//	+ "\n\tArchive header:\t" + std::to_string(headerSize) + " B (" + std::to_string(int(100.0f * float(headerSize) / float(totalSize))) + " %)"
		//	+ "\n\tValue headers:\t" + std::to_string(valueHeaders.getSize()) + " B (" + std::to_string(int(100.0f * float(valueHeaders.getSize()) / float(totalSize))) + " %)"
		//	+ "\n\tValue data:\t" + std::to_string(dataSize) + " B (" + std::to_string(int(100.0f * float(dataSize) / float(totalSize))) + " %)"
		//	+ "\n\tContainers:\t" + std::to_string(containers.getSize()) + " B (" + std::to_string(int(100.0f * float(containers.getSize()) / float(totalSize))) + " %)"
		//	);

		return data;
	}
}
