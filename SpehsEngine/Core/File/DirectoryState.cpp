#include "stdafx.h"
#include "SpehsEngine/Core/File/DirectoryState.h"

#include "SpehsEngine/Core/File/FileSystem.h"
#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/Murmur3.h"
#include <boost/filesystem.hpp>

namespace se
{
	void DirectoryState::FileState::write(se::WriteBuffer& writeBuffer) const
	{
		writeToBuffer<uint32_t>(writeBuffer, path);
		writeBuffer.write(dataHash);
	}

	bool DirectoryState::FileState::read(se::ReadBuffer& readBuffer)
	{
		if (!readFromBuffer<uint32_t>(readBuffer, path))
		{
			return false;
		}
		se_read(readBuffer, dataHash);
		return true;
	}

	void DirectoryState::write(se::WriteBuffer& writeBuffer) const
	{
		writeToBuffer<uint32_t>(writeBuffer, path);
		writeToBuffer<FileState, uint32_t>(writeBuffer, files);
		writeToBuffer<DirectoryState, uint32_t>(writeBuffer, directories);
	}

	bool DirectoryState::read(se::ReadBuffer& readBuffer)
	{
		if (!readFromBuffer<uint32_t>(readBuffer, path))
		{
			return false;
		}
		if (!readFromBuffer<FileState, uint32_t>(readBuffer, files))
		{
			return false;
		}
		if (!readFromBuffer<DirectoryState, uint32_t>(readBuffer, directories))
		{
			return false;
		}
		return true;
	}

	const DirectoryState::FileState* DirectoryState::findFileState(std::string filePath) const
	{
		while (filePath.size() > 0 && filePath.front() == '/')
		{
			filePath.erase(filePath.begin());
		}

		if (filePath.empty())
		{
			return nullptr;
		}

		std::string subDirectoryPath;
		for (size_t i = 0; i < filePath.size(); i++)
		{
			if (filePath[i] == '/')
			{
				subDirectoryPath.resize(i);
				if (i > 0)
				{
					memcpy(&subDirectoryPath[0], filePath.data(), i);
					filePath.erase(filePath.begin(), filePath.begin() + i);
				}
				break;
			}
		}

		if (subDirectoryPath.empty())
		{
			for (size_t i = 0; i < files.size(); i++)
			{
				if (files[i].path == filePath)
				{
					return &files[i];
				}
			}
			return nullptr;
		}
		else
		{
			for (size_t i = 0; i < directories.size(); i++)
			{
				if (directories[i].path == subDirectoryPath)
				{
					return directories[i].findFileState(filePath);
				}
			}
			return nullptr;
		}
	}

	void getDirectoryStateImpl(DirectoryState& directoryState, const std::string& fullDirectoryPath, const std::string& relativeDirectoryPath, const DirectoryState::Flag::Type flags, const size_t fileDataHashSeed)
	{
		directoryState.path = relativeDirectoryPath;
		const std::vector<std::string> files = se::listFilesInDirectory(fullDirectoryPath);
		directoryState.files.clear();
		directoryState.directories.clear();
		for (size_t i = 0; i < files.size(); i++)
		{
			const std::string fullFilePath = fullDirectoryPath.empty() ? files[i] : (fullDirectoryPath + "/" + files[i]);
			const std::string relativeFilePath = relativeDirectoryPath.empty() ? files[i] : (relativeDirectoryPath + "/" + files[i]);
			if (se::isDirectory(fullFilePath))
			{
				directoryState.directories.push_back(DirectoryState());
				getDirectoryStateImpl(directoryState.directories.back(), fullFilePath, relativeFilePath, flags, fileDataHashSeed);
			}
			else
			{
				directoryState.files.push_back(DirectoryState::FileState());
				directoryState.files.back().path = relativeFilePath;
				if (flags & (DirectoryState::Flag::read | DirectoryState::Flag::hash))
				{
					File file;
					if (readFile(file, fullFilePath))
					{
						if (flags & DirectoryState::Flag::hash)
						{
							directoryState.files.back().dataHash = murmurHash3_x86_32(file.data.data(), file.data.size(), fileDataHashSeed);
						}
						if (flags & DirectoryState::Flag::read)
						{
							directoryState.files.back().data.swap(file.data);
						}
					}
				}
			}
		}
	}

	void getDirectoryState(DirectoryState& directoryState, const std::string& path, const DirectoryState::Flag::Type flags, const size_t fileDataHashSeed)
	{
		getDirectoryStateImpl(directoryState, path, "", flags, fileDataHashSeed);
	}

	void getDirectoriesAndFiles(const DirectoryState& directoryState, std::vector<std::string>& directories, std::vector<std::string>& files)
	{
		for (size_t i = 0; i < directoryState.files.size(); i++)
		{
			files.push_back(directoryState.files[i].path);
		}
		for (size_t i = 0; i < directoryState.directories.size(); i++)
		{
			directories.push_back(directoryState.directories[i].path);
		}
	}

	void compare(const DirectoryState& directoryState, const std::string& path, const size_t fileDataHashSeed, std::vector<std::string>& missingDirectories, std::vector<std::string>& missingFiles, std::vector<std::string>& unequalFiles)
	{
		for (size_t i = 0; i < directoryState.files.size(); i++)
		{
			const std::string filePath = path.empty() ? directoryState.files[i].path : path + "/" + directoryState.files[i].path;
			File file;
			if (readFile(file, filePath))
			{
				if (directoryState.files[i].dataHash != 0u)
				{
					const uint32_t dataHash = murmurHash3_x86_32(file.data.data(), file.data.size(), fileDataHashSeed);
					if (dataHash != directoryState.files[i].dataHash)
					{
						unequalFiles.push_back(directoryState.files[i].path);
					}
				}
				else
				{
					se::log::error("DirectoryState file data hashes must be pre-calculated.");
				}
			}
			else
			{
				missingFiles.push_back(directoryState.files[i].path);
			}
		}
		for (size_t i = 0; i < directoryState.directories.size(); i++)
		{
			const std::string directoryPath = path.empty() ? directoryState.directories[i].path : path + "/" + directoryState.directories[i].path;
			if (!fileExists(directoryPath) || !isDirectory(directoryPath))
			{
				missingDirectories.push_back(directoryState.directories[i].path);
				getDirectoriesAndFiles(directoryState.directories[i], missingDirectories, missingFiles);
			}
			else
			{
				compare(directoryState.directories[i], path, fileDataHashSeed, missingDirectories, missingFiles, unequalFiles);
			}
		}
	}
}
