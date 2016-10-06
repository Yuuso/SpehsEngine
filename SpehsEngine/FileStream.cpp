#include <mutex>
#include <boost/filesystem.hpp>
#include "FileStream.h"
#include "Console.h"

namespace spehs
{
	std::recursive_mutex filestreamMutex;


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
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		if (boost::filesystem::exists(path))
		{//File exists, check type
			if (boost::filesystem::is_directory(path))
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool fileExists(std::string& path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		if (boost::filesystem::exists(path))
		{//File exists, check type
			if (boost::filesystem::is_regular_file(path))
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool removeFile(std::string& path)
	{
		return boost::filesystem::remove(path);
	}

	bool createDirectory(std::string& path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		return boost::filesystem::create_directory(path);
	}

	bool verifyDirectory(std::string& path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		if (!spehs::directoryExists(path))
		{
			if (!spehs::createDirectory(path))
			{
				spehs::console::warning("Failed to create directory: \"" + path + "\"!");
				return false;
			}
		}
		return true;
	}
	
	std::vector<std::string> listFilesInDirectory(std::string directoryPath, std::string fileType)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		std::vector<std::string> files;
		boost::filesystem::path path(directoryPath);
		if (!boost::filesystem::exists(path))
		{
			spehs::console::warning("listFilesInDirectory failed: directory does not exist!");
			return files;
		}
		if (!boost::filesystem::is_directory(path))
		{
			spehs::console::warning("listFilesInDirectory failed: directory path leads to a non-directory file!");
			return files;
		}

		if (fileType.size() > 0 && fileType[0] == '.')
			fileType.erase(fileType.begin());

		boost::filesystem::directory_iterator it(path);
		boost::filesystem::directory_iterator end;
		while (it != end)
		{
			std::string fileName(it->path().filename().generic_string());
			if (fileType.size() > 0)
			{//Looking for specific file type
				std::string type;
				for (unsigned i = 0; i < fileName.size(); i++)
				{
					if (fileName[i] == '.' &&
						i < fileName.size() - 1/*There are still some characters to read*/)
					{
						type.resize(fileName.size() - i - 1);
						memcpy(&type[0], &fileName[i + 1], sizeof(char)* (fileName.size() - i - 1));
						if (type == fileType)
						{//Correct type

							//Copy file name without the file type into type
							type.resize(i);
							memcpy(&type[0], &fileName[0], sizeof(char) * i);
							files.push_back(type);
						}
						break;
					}
				}
			}
			else//Add all found files
				files.push_back(fileName);
			it++;
		}

		return files;
	}
}