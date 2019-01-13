#include "stdafx.h"
#include "SpehsEngine/Core/File/FileSystem.h"

#include <mutex>
#include <boost/filesystem.hpp>

namespace
{
	static std::recursive_mutex filestreamMutex;
}

namespace se
{
	bool fileExists(const std::string& path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		if (boost::filesystem::exists(path))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool isDirectory(const std::string& path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		if (boost::filesystem::exists(path))
		{
			if (boost::filesystem::is_regular_file(path))
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	bool removeFile(const std::string& path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		return boost::filesystem::remove(path);
	}

	bool createDirectory(const std::string& path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		return boost::filesystem::create_directory(path);
	}
	
	bool verifyDirectory(const std::string& path)
	{
		if (fileExists(path))
		{
			if (isDirectory(path))
			{
				return true;
			}
			else
			{
				//Another non-directory exists at the given path
				return false;
			}
		}
		else
		{
			return createDirectory(path);
		}
	}
	
	std::vector<std::string> listFilesInDirectory(const std::string& directoryPath, std::string fileType)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		std::vector<std::string> files;
		const boost::filesystem::path path(directoryPath.empty() ? getCurrentPath() : directoryPath);
		if (directoryPath.size() > 0)
		{
			if (!boost::filesystem::exists(path))
			{
				log::warning("listFilesInDirectory failed: directory does not exist! : " + directoryPath);
				return files;
			}
			if (!boost::filesystem::is_directory(path))
			{
				log::warning("listFilesInDirectory failed: directory path leads to a non-directory file! : " + directoryPath);
				return files;
			}
		}

		if (fileType.size() > 0 && fileType[0] == '.')
		{
			fileType.erase(fileType.begin());
		}

		boost::filesystem::directory_iterator it(path);
		boost::filesystem::directory_iterator end;
		while (it != end)
		{
			std::string fileName(it->path().filename().generic_string());
			if (fileType.size() > 0)
			{
				//Looking for specific file type
				std::string type;
				for (unsigned i = 0; i < fileName.size(); i++)
				{
					if (fileName[i] == '.' && i < fileName.size() - 1/*There are still some characters to read*/)
					{
						type.resize(fileName.size() - i - 1);
						memcpy(&type[0], &fileName[i + 1], sizeof(char)* (fileName.size() - i - 1));
						if (type == fileType)
						{
							//Copy file name without the file type into type
							type.resize(i);
							memcpy(&type[0], &fileName[0], sizeof(char) * i);
							files.push_back(type);
						}
						break;
					}
				}
			}
			else
			{
				files.push_back(fileName);
			}
			it++;
		}

		return files;
	}

	std::vector<std::string> listSubDirectoriesInDirectory(const std::string& directoryPath)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		std::vector<std::string> subDirectories;
		const boost::filesystem::path path(directoryPath.empty() ? getCurrentPath() : directoryPath);
		if (directoryPath.size() > 0)
		{
			if (!boost::filesystem::exists(path))
			{
				log::warning("listSubDirectoriesInDirectory failed: directory does not exist! : " + directoryPath);
				return subDirectories;
			}
			if (!boost::filesystem::is_directory(path))
			{
				log::warning("listSubDirectoriesInDirectory failed: directory path leads to a non-directory file! : " + directoryPath);
				return subDirectories;
			}
		}
		
		boost::filesystem::directory_iterator it(path);
		boost::filesystem::directory_iterator end;
		while (it != end)
		{
			std::string fileName(it->path().filename().generic_string());
			if (boost::filesystem::is_directory(directoryPath + fileName))
			{
				subDirectories.push_back(fileName);
			}
			it++;
		}

		return subDirectories;
	}

	std::string getCurrentPath()
	{
		return boost::filesystem::current_path().generic_string();
	}
}
