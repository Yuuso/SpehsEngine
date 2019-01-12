#include "stdafx.h"
#include "SpehsEngine/Core/File/DirectoryState.h"

#include "SpehsEngine/Core/File/FileSystem.h"
#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
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

	void getDirectoryState(DirectoryState& directory, const std::string& path, const size_t fileDataHashSeed)
	{
		directory.path = path;
		const std::vector<std::string> files = se::listFilesInDirectory(path);
		directory.files.clear();
		directory.directories.clear();
		for (size_t i = 0; i < files.size(); i++)
		{
			const std::string fullFilePath = path + "/" + files[i];
			if (se::isDirectory(fullFilePath))
			{
				directory.directories.push_back(DirectoryState());
				getDirectoryState(directory.directories.back(), path + "/" + files[i], fileDataHashSeed);
			}
			else
			{
				directory.files.push_back(DirectoryState::FileState());
				directory.files.back().path = fullFilePath;
				if (fileDataHashSeed)
				{
					File file;
					if (readFile(file, fullFilePath))
					{
						directory.files.back().dataHash = murmurHash3_x86_32(file.data.data(), file.data.size(), fileDataHashSeed);
					}
				}
			}
		}
	}
}
