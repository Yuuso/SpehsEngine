#include "stdafx.h"
#include "SpehsEngine/Core/File/File.h"

#include <fstream>
#include "SpehsEngine/Core/File/FileSystem.h"


namespace se
{
	bool readFile(File& file, const std::string_view path)
	{
		std::ifstream stream;
		stream.open(path.data(), std::ios::binary);
		if (stream.fail())
		{
			log::warning("Failed to load file at: " + std::string(path));
			return false;
		}
		stream.seekg(0, std::ios::end);
		const size_t streamSize = size_t(stream.tellg());
		stream.seekg(0, std::ios::beg);
		if (streamSize == 0)
		{
			log::warning("Failed to load file at: " + std::string(path) + ". File size is 0.");
			return false;
		}
		file.data.resize(streamSize);
		stream.read((char*)file.data.data(), file.data.size());
		if (stream.bad())
		{
			log::error("Failed to read file at: " + std::string(path) + ". std::ifstream read failed.");
			return false;
		}
		return true;
	}

	bool writeFile(const File& file)
	{
		std::ofstream stream;
		stream.open(file.path, std::ios::binary);
		if (stream.fail())
		{
			se::log::warning("Failed to write file at: " + file.path + ". Failed to open file.");
			return false;
		}

		stream.write((char*)file.data.data(), file.data.size());
		if (stream.fail())
		{
			se::log::warning("Failed to write file at: " + file.path + ". Failure occured while writing.");
			return false;
		}
		return true;
	}
}
