#include "stdafx.h"
#include "SpehsEngine/Core/File/File.h"

#include <fstream>
#include "SpehsEngine/Core/File/FileSystem.h"

#pragma optimize("", off) // nocommit
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
		file.path = path;
		return true;
	}

	bool writeFile(const File& file)
	{
		std::ofstream stream;
		stream.open(file.path, std::ios::binary);
		if (stream.fail())
		{
			// Attempt to create the directory
			bool directoryVerified = false;
			for (size_t i = file.path.size(); i-- > 0;)
			{
				if ((file.path[i] == '/' || file.path[i] == '\\' && i > 0))
				{
					const std::string directoryPath(file.path.data(), i);
					//const std::string directoryPath(file.path.rend(), it);
					if (!directoryPath.empty())
					{
						if (verifyDirectory(directoryPath))
						{
							directoryVerified = true;
						}
						else
						{
							return false;
						}
					}
					break;
				}
			}

			if (!directoryVerified)
			{
				return false;
			}

			stream.open(file.path, std::ios::binary);
			if (stream.fail())
			{
				se::log::warning("Failed to write file at: " + file.path + ". Failed to open file.");
				return false;
			}
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
