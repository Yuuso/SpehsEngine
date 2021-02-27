#pragma once

#include <string>
#include <vector>


namespace se
{
	struct File
	{
		std::string path;
		std::vector<uint8_t> data;
	};

	// Opens file, retrieves file contents and closes file. Returns false on failure.
	bool readFile(File& _file);
	bool readFile(File& _file, const std::string& _path);
	bool readFile(const std::string& _path, uint8_t* _data, const size_t _dataSize);

	// Opens file and truncates previous contents with the given data.
	bool writeFile(const File& _file);
	bool writeFile(const std::string& _path, const uint8_t* _data, const size_t _dataSize);
}
