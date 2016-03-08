#include <boost/filesystem.hpp>
#include "FileStream.h"
#include "Console.h"
using namespace boost::filesystem;

namespace spehs
{
	static unsigned count;
	void writeString(std::string& string, std::ofstream* stream)
	{
		count = string.size();
		stream->write((char*)&count, sizeof(count));
		for (unsigned i = 0; i < count; i++)
			stream->write((char*)&string[i], sizeof(char));
	}
	void readString(std::string& string, std::ifstream* stream)
	{
		stream->read((char*)&count, sizeof(count));
		string.resize(count);
		for (unsigned i = 0; i < count; i++)
			stream->read((char*)&string[i], sizeof(char));
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