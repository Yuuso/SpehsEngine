#include <boost/filesystem.hpp>
#include "FileStream.h"
#include "Console.h"
using namespace boost::filesystem;

namespace spehs
{
	void writeString(std::string& string, std::ofstream* stream)
	{
		unsigned count = string.size();
		stream->write((char*)&count, sizeof(count));
		for (unsigned i = 0; i < count; i++)
			stream->write((char*)&string[i], sizeof(char));
	}
	size_t writeString(std::string& string, unsigned char* buffer)
	{
		unsigned count = string.size();
		memcpy(buffer, &count, sizeof(count));
		size_t offset = sizeof(count);
		for (unsigned i = 0; i < count; i++)
		{
			memcpy(&buffer[offset], &string[i], sizeof(char));
			offset += sizeof(char);
		}
		return offset;
	}

	void readString(std::string& string, std::ifstream* stream)
	{
		unsigned count;
		stream->read((char*)&count, sizeof(count));
		string.resize(count);//Could potentially invalidate string reference??
		for (unsigned i = 0; i < count; i++)
			stream->read((char*)&string[i], sizeof(char));
	}
	size_t readString(std::string& string, unsigned char* buffer)
	{
		unsigned count;
		size_t offset = 0;
		memcpy(&count, &buffer[offset], sizeof(count));
		offset += sizeof(count);
		string.resize(count);//Could potentially invalidate string reference??
		memcpy(&string[0], &buffer[offset], sizeof(char) * count);
		offset += sizeof(char) * count;
		return offset;
	}

	bool directoryExists(std::string& path)
	{
		if (exists(path))
		{//File exists, check type
			if (is_directory(path))
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool fileExists(std::string& path)
	{
		if (exists(path))
		{//File exists, check type
			if (is_regular_file(path))
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool createDirectory(std::string& path)
	{
		return create_directory(path);
	}

	bool verifyDirectory(std::string& path)
	{
		if (!spehs::directoryExists(path))
		{
			if (!spehs::createDirectory(path))
			{
				spehs::console::log("Failed to create directory: \"" + path + "\"!");
				return false;
			}
		}
		return true;
	}
}