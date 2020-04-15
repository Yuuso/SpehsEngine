#include "stdafx.h"
#include "SpehsEngine/Core/ArchiveUtilityFunctions.h"

#include "SpehsEngine/Core/File/File.h"

namespace se
{
	bool writeToFile(const se::Archive& archive, const std::string& filepath)
	{
		se::WriteBuffer writeBuffer;
		writeBuffer.write(archive);
		se::File file;
		writeBuffer.swap(file.data);
		file.path = filepath;
		return writeFile(file);
	}

	bool readFromFile(se::Archive& archive, const std::string& filepath)
	{
		se::File file;
		if (se::readFile(file, filepath))
		{
			se::ReadBuffer readBuffer(file.data.data(), uint32_t(file.data.size()));
			return archive.read(readBuffer);
		}
		else
		{
			return false;
		}
	}
}
