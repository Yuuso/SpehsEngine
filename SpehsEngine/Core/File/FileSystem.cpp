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
	bool fileExists(const std::string& _path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		if (boost::filesystem::exists(_path))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool isDirectory(const std::string& _path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		if (boost::filesystem::exists(_path))
		{
			if (boost::filesystem::is_regular_file(_path))
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

	size_t fileSize(const std::string& _path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		try
		{
			return static_cast<size_t>(boost::filesystem::file_size(_path));
		}
		catch (boost::filesystem::filesystem_error& /*error*/)
		{
			log::warning("Error when trying to get file size of '" + _path + "'");
			return 0u;
		}
	}

	bool removeFile(const std::string& _path)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		boost::system::error_code error;
		const bool result = boost::filesystem::remove(_path, error);
		if (error)
		{
			log::warning("Error when trying to remove the file '" + _path + "' : " + error.message());
		}
		return result;
	}

	bool renameFile(const std::string& _oldPath, const std::string& _newPath)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);

		// Verify directory
		size_t directoryCharSearchCount = 0;
		for (std::string::const_reverse_iterator it = _newPath.rbegin(); it != _newPath.rend(); it++)
		{
			directoryCharSearchCount++;
			if (*it == '/' || *it == '\\')
			{
				std::string directoryPath(_newPath, _newPath.size() - directoryCharSearchCount);
				if (verifyDirectory(directoryPath))
				{
					break;
				}
				else
				{
					return false;
				}
			}
		}

		boost::system::error_code error;
		boost::filesystem::rename(_oldPath, _newPath, error);
		if (error)
		{
			log::warning(error.message());
			return false;
		}
		else
		{
			return true;
		}
	}

	bool createDirectory(const std::string& _fullPath)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		// NOTE: seems that boost isn't able to create multiple directories at a time if more than one new directories are required
		auto verifyPath = [](const std::string& _path)
		{
			if (fileExists(_path))
			{
				if (isDirectory(_path))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return boost::filesystem::create_directory(_path);
			}
		};
		for (size_t i = 0; i < _fullPath.size(); i++)
		{
			if (_fullPath[i] == '/' || _fullPath[i] == '\\' && i != _fullPath.size() - 1)
			{
				std::string partialPath;
				partialPath.insert(partialPath.begin(), _fullPath.begin(), _fullPath.begin() + i);
				if (!verifyPath(partialPath))
				{
					return false;
				}
			}
		}
		return boost::filesystem::create_directory(_fullPath);
	}

	bool verifyDirectory(const std::string& _path)
	{
		if (fileExists(_path))
		{
			if (isDirectory(_path))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return createDirectory(_path);
		}
	}

	std::vector<std::string> listFilesInDirectory(const std::string& _directoryPath, std::string _fileType)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		std::vector<std::string> files;
		const boost::filesystem::path path((_directoryPath.empty() ? getCurrentPath() : _directoryPath));
		if (_directoryPath.size() > 0)
		{
			if (!boost::filesystem::exists(path))
			{
				log::warning("listFilesInDirectory failed: directory does not exist! : " + _directoryPath);
				return files;
			}
			if (!boost::filesystem::is_directory(path))
			{
				log::warning("listFilesInDirectory failed: directory path leads to a non-directory file! : " + _directoryPath);
				return files;
			}
		}

		if (_fileType.size() > 0 && _fileType[0] == '.')
		{
			_fileType.erase(_fileType.begin());
		}

		boost::filesystem::directory_iterator it(path);
		boost::filesystem::directory_iterator end;
		while (it != end)
		{
			std::string fileName(it->path().filename().generic_string());
			if (_fileType.size() > 0)
			{
				//Looking for specific file type
				std::string type;
				for (unsigned i = 0; i < fileName.size(); i++)
				{
					if (fileName[i] == '.' && i < fileName.size() - 1/*There are still some characters to read*/)
					{
						type.resize(fileName.size() - i - 1);
						memcpy(&type[0], &fileName[i + 1], sizeof(char)* (fileName.size() - i - 1));
						if (type == _fileType)
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

	std::vector<std::string> listSubDirectoriesInDirectory(std::string _directoryPath)
	{
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		std::vector<std::string> subDirectories;
		if (_directoryPath.empty())
		{
			_directoryPath = getCurrentPath();
		}
		if (_directoryPath.size() > 0 && _directoryPath.back() != '/' && _directoryPath.back() != '\\')
		{
			_directoryPath += '/';
		}
		const boost::filesystem::path path(_directoryPath);
		if (_directoryPath.size() > 0)
		{
			if (!boost::filesystem::exists(path))
			{
				log::warning("listSubDirectoriesInDirectory failed: directory does not exist! : " + _directoryPath);
				return subDirectories;
			}
			if (!boost::filesystem::is_directory(path))
			{
				log::warning("listSubDirectoriesInDirectory failed: directory path leads to a non-directory file! : " + _directoryPath);
				return subDirectories;
			}
		}

		boost::filesystem::directory_iterator it(path);
		boost::filesystem::directory_iterator end;
		while (it != end)
		{
			std::string fileName(it->path().filename().generic_string());
			if (boost::filesystem::is_directory(_directoryPath + fileName))
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

	file::Timestamp getFileTimestamp(const std::string& _filepath)
	{
		const boost::filesystem::path path(_filepath);
		const time_t time = boost::filesystem::last_write_time(path);
		return file::Timestamp(time);
	}

	void setFileTimestamp(const std::string& filepath, const file::Timestamp timestamp)
	{
		const time_t time = timestamp.value;
		const boost::filesystem::path path(filepath);
		boost::filesystem::last_write_time(path, time);
	}

	uint64_t getFileSize(const std::string& path)
	{
		const boost::filesystem::path boostPath(path.data());
		std::lock_guard<std::recursive_mutex> lock(filestreamMutex);
		return uint64_t(boost::filesystem::file_size(boostPath));
	}
}
