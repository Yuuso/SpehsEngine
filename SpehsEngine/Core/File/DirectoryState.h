#pragma once
#include <string>
#include <vector>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	struct DirectoryState
	{
		struct Flag
		{
			enum Type : uint8_t
			{
				read = 0x01,
				hash = 0x02,
			};
		};
		struct FileState
		{
			void write(se::WriteBuffer& writeBuffer) const;
			bool read(se::ReadBuffer& readBuffer);
			std::string path;
			std::vector<uint8_t> data;
			/* The file hash is a hash value that is computed using the file's local (DirectoryState) path and file data contents. */
			uint32_t fileHash = 0u;
		};
		void write(se::WriteBuffer& writeBuffer) const;
		bool read(se::ReadBuffer& readBuffer);
		const FileState* findFileState(std::string filePath) const;
		const FileState* findFileState(const uint32_t fileHash) const;
		std::string path;
		std::vector<FileState> files;
		std::vector<DirectoryState> directories;
	};
	
	/*
		Returns directory state for given path recursively.
		Returned DirectoryState paths will be relative to the given path.
	*/
	void getDirectoryState(DirectoryState& directoryState, const std::string& path, const DirectoryState::Flag::Type flags, const size_t fileHashSeed);

	void getDirectoriesAndFiles(const DirectoryState& directoryState, std::vector<std::string>& directories, std::vector<std::string>& files);
	
	/*
		Compares given directory state against given path.
		Detects missing directories, files and unequal (data hash comparison) files.
		DirectoryState file hashes must be pre-calculated.
	*/
	void compare(const DirectoryState& directoryState, const std::string& path, const size_t fileHashSeed, std::vector<std::string>& missingDirectories, std::vector<std::string>& missingFiles, std::vector<std::string>& unequalFiles);
}
