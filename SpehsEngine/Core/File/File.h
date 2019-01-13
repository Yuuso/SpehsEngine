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

	/* Opens file, retrieves file contents and closes file. Returns false on failure. */
	bool readFile(File& file, const std::string& path);

	/* Opens file and truncates previous contents with the given data. */
	bool writeFile(const File& file);

	uint32_t getFileDataHash(const File& file, const size_t seed);
}
