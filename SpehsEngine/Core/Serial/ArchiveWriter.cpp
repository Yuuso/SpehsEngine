#include "stdafx.h"
#include "SpehsEngine/Core/Serial/ArchiveWriter.h"


namespace se
{
	std::vector<uint8_t> ArchiveWriter::generateData() const
	{
		constexpr size_t objectHeaderSize = sizeof(uint64_t) + sizeof(uint32_t);
		se_assert(archiveIdStack.empty() && "You should not call this function while objects are being written");
		BinaryWriter binaryWriter;
		uint8_t version = 1; // Current version
		const uint32_t objectCount = uint32_t(binaryWriter1.getSize() / objectHeaderSize);
		se_assert((objectCount * objectHeaderSize) == binaryWriter1.getSize());
		const uint32_t buffersSize = uint32_t(binaryWriter1.getSize() + binaryWriter2.getSize());
		const size_t totalSize = sizeof(version) + sizeof(objectCount) + buffersSize;
		{
			// Single reserve call to cover the entire write
			std::vector<uint8_t> data;
			data.reserve(totalSize);
			binaryWriter.swap(data);
		}

		binaryWriter.serial(version);
		binaryWriter.serial(objectCount);

		// Write the object data blocks in a single go
		std::vector<uint8_t> data;
		binaryWriter.swap(data);
		size_t offset = data.size();
		data.resize(totalSize);
		memcpy(data.data() + offset, binaryWriter1.getData(), binaryWriter1.getSize());	offset += binaryWriter1.getSize();
		memcpy(data.data() + offset, binaryWriter2.getData(), binaryWriter2.getSize());
		return data;
	}
}
