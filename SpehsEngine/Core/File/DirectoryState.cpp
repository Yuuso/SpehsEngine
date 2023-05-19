#include "stdafx.h"
#include "SpehsEngine/Core/File/DirectoryState.h"

#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Core/File/FileSystem.h"
#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Core/BitwiseOperations.h"


namespace se
{
	const DirectoryState::FileState* DirectoryState::findFileState(std::string remainingFilePath) const
	{
		while (remainingFilePath.size() > 0 && remainingFilePath.front() == '/')
		{
			remainingFilePath.erase(remainingFilePath.begin());
		}

		if (remainingFilePath.empty())
		{
			return nullptr;
		}

		for (size_t p = 0; p < remainingFilePath.size(); p++)
		{
			if (remainingFilePath[p] == '/')
			{
				std::string subDirectoryPath = path.empty() ? "" : path + '/';
				subDirectoryPath.insert(subDirectoryPath.end(), remainingFilePath.begin(), remainingFilePath.begin() + p);
				remainingFilePath.erase(remainingFilePath.begin(), remainingFilePath.begin() + p + 1);
				for (size_t d = 0; d < directories.size(); d++)
				{
					if (directories[d].path == subDirectoryPath)
					{
						return directories[d].findFileState(remainingFilePath);
					}
				}
				return nullptr;
			}
		}

		const std::string fullFilePath = path.empty() ? remainingFilePath : path + '/' + remainingFilePath;
		for (size_t i = 0; i < files.size(); i++)
		{
			if (files[i].path == fullFilePath)
			{
				return &files[i];
			}
		}
		return nullptr;
	}

	const DirectoryState::FileState* DirectoryState::findFileState(const uint32_t fileHash) const
	{
		for (size_t i = 0; i < files.size(); i++)
		{
			if (files[i].fileHash == fileHash)
			{
				return &files[i];
			}
		}
		for (size_t i = 0; i < directories.size(); i++)
		{
			if (const DirectoryState::FileState* const fileState = directories[i].findFileState(fileHash))
			{
				return fileState;
			}
		}
		return nullptr;
	}

	void getDirectoryStateImpl(DirectoryState& directoryState, const std::string& fullDirectoryPath, const std::string& relativeDirectoryPath,
		const DirectoryState::Flag flags, const uint32_t fileHashSeed)
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
				getDirectoryStateImpl(directoryState.directories.back(), fullFilePath, relativeFilePath, flags, fileHashSeed);
			}
			else
			{
				directoryState.files.push_back(DirectoryState::FileState());
				directoryState.files.back().path = relativeFilePath;
				if (checkBit(flags, DirectoryState::Flag::Read) || checkBit(flags, DirectoryState::Flag::Hash))
				{
					File file;
					if (readFile(file, fullFilePath))
					{
						directoryState.files.back().fileSize = uint64_t(file.data.size());
						if (checkBit(flags, DirectoryState::Flag::Hash))
						{
							std::vector<uint8_t> hashData(directoryState.files.back().path.size() + file.data.size());
							memcpy(&hashData[0], directoryState.files.back().path.data(), directoryState.files.back().path.size());
							memcpy(&hashData[directoryState.files.back().path.size()], file.data.data(), file.data.size());
							directoryState.files.back().fileHash = murmurHash3_x86_32(hashData.data(), hashData.size(), fileHashSeed);
						}
						if (checkBit(flags, DirectoryState::Flag::Read))
						{
							directoryState.files.back().data.swap(file.data);
						}
					}
				}
				else if (checkBit(flags, DirectoryState::Flag::Read))
				{
					directoryState.files.back().fileSize = getFileSize(fullFilePath);
				}
			}
		}
	}

	void getDirectoryState(DirectoryState& directoryState, const std::string& path, const DirectoryState::Flag flags, const uint32_t fileHashSeed)
	{
		getDirectoryStateImpl(directoryState, path, "", flags, fileHashSeed);
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
		for (size_t i = 0; i < directoryState.directories.size(); i++)
		{
			getDirectoriesAndFiles(directoryState.directories[i], directories, files);
		}
	}

	void compare(const DirectoryState& directoryState, const std::string& path, const uint32_t fileHashSeed,
		std::vector<std::string>& missingDirectories,
		std::vector<std::string>& missingFiles,
		std::vector<std::string>& unequalFiles)
	{
		for (size_t i = 0; i < directoryState.files.size(); i++)
		{
			const std::string filePath = path.empty() ? directoryState.files[i].path : path + "/" + directoryState.files[i].path;
			File file;
			if (readFile(file, filePath))
			{
				if (directoryState.files[i].fileHash != 0u)
				{
					std::vector<uint8_t> hashData(directoryState.files[i].path.size() + file.data.size());
					memcpy(&hashData[0], directoryState.files[i].path.data(), directoryState.files[i].path.size());
					memcpy(&hashData[directoryState.files[i].path.size()], file.data.data(), file.data.size());
					const uint32_t fileHash = murmurHash3_x86_32(hashData.data(), hashData.size(), fileHashSeed);
					if (fileHash != directoryState.files[i].fileHash)
					{
						unequalFiles.push_back(directoryState.files[i].path);
					}
				}
				else
				{
					se::log::error("DirectoryState file hashes must be pre-calculated.");
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
				compare(directoryState.directories[i], path, fileHashSeed, missingDirectories, missingFiles, unequalFiles);
			}
		}
	}
}
