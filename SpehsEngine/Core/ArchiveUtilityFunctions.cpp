#include "stdafx.h"
#include "SpehsEngine/Core/ArchiveUtilityFunctions.h"

#include "SpehsEngine/Core/File/File.h"


namespace se
{
	bool writeToFile(const std::string_view filepath, const se::Archive& archive)
	{
		se::WriteBuffer writeBuffer;
		writeBuffer.write(archive);
		se::File file;
		writeBuffer.swap(file.data);
		file.path = filepath;
		return writeFile(file);
	}

	bool readFromFile(const std::string_view filepath, se::Archive& archive)
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
