#include "stdafx.h"
#include "SpehsEngine/Core/File/File.h"

#include <fstream>
#include "SpehsEngine/Core/File/FileSystem.h"

#pragma optimize("", off) // nocommit
namespace se
{
	bool readFile(File& _file)
	{
		se_assert(!_file.path.empty());
		return readFile(_file, _file.path);
	}
	bool readFile(File& _file, const std::string& _path)
	{
		std::ifstream stream;
		stream.open(_path, std::ios::binary);
		if (stream.fail())
		{
			log::warning("Failed to load file at: " + _path);
			return false;
		}

		const size_t streamSize = fileSize(_path);
		if (streamSize == 0u)
		{
			log::warning("Failed to load file at: " + _path + ". File size is 0.");
			return false;
		}

		_file.data.resize(streamSize);
		stream.read(reinterpret_cast<char*>(_file.data.data()), _file.data.size());
		if (!stream.good())
		{
			log::error("Failed to read file at: " + _path + ". std::ifstream read failed.");
			return false;
		}
		if (&_file.path != &_path)
			_file.path = _path;
		return true;
	}
	bool readFile(const std::string& _path, uint8_t* _data, const size_t _dataSize)
	{
		std::ifstream stream;
		stream.open(_path, std::ios::binary);
		if (stream.fail())
		{
			log::warning("Failed to load file at: " + _path);
			return false;
		}

		const size_t streamSize = fileSize(_path);
		if (streamSize == 0u)
		{
			log::warning("Failed to load file at: " + _path + ". File size is 0.");
			return false;
		}

		stream.read(reinterpret_cast<char*>(_data), _dataSize);
		if (!stream.good())
		{
			log::error("Failed to read file at: " + _path + ". std::ifstream read failed.");
			return false;
		}
		return true;
	}

	bool writeFile(const File& _file)
	{
		return writeFile(_file.path, _file.data.data(), _file.data.size());
	}

	bool writeFile(const std::string& _path, const uint8_t* _data, const size_t _dataSize)
	{
		std::ofstream stream;
		stream.open(_path, std::ios::binary);
		if (stream.fail())
		{
			// Attempt to create the directory
			bool directoryVerified = false;
			for (size_t i = _path.size(); i-- > 0;)
			{
				if ((_path[i] == '/' || _path[i] == '\\' && i > 0))
				{
					const std::string directoryPath(_path.data(), i);
					//const std::string directoryPath(_path.rend(), it);
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

			stream.open(_path, std::ios::binary);
			if (stream.fail())
			{
				se::log::warning("Failed to write file at: " + _path + ". Failed to open file.");
				return false;
			}
		}

		stream.write(reinterpret_cast<const char*>(_data), _dataSize);
		if (stream.fail())
		{
			se::log::warning("Failed to write file at: " + _path + ". Failure occured while writing.");
			return false;
		}
		return true;
	}
}
