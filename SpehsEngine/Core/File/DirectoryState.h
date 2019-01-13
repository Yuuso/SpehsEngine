#pragma once
#include <string>
#include <vector>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	struct DirectoryState
	{
		struct FileState
		{
			void write(se::WriteBuffer& writeBuffer) const;
			bool read(se::ReadBuffer& readBuffer);
			std::string path;
			/* The data hash is the hash value of the file contents. */
			uint32_t dataHash = 0u;
		};
		void write(se::WriteBuffer& writeBuffer) const;
		bool read(se::ReadBuffer& readBuffer);
		std::string path;
		std::vector<FileState> files;
		std::vector<DirectoryState> directories;
	};

	/*
		Returns directory state for given path recursively.
		Returned DirectoryState paths will be relative to the given path.
		If fileDataHashSeed is set to non-zero, each found file will be opened and data hash will be computed.
	*/
	void getDirectoryState(DirectoryState& directoryState, const std::string& path, const size_t fileDataHashSeed = 0);

	void getDirectoriesAndFiles(const DirectoryState& directoryState, std::vector<std::string>& directories, std::vector<std::string>& files);

	/*
		Compares given directory state against given path.
		Detects missing directories, files and unequal (data hash comparison) files.
		DirectoryState file data hashes must be pre-calculated.
	*/
	void compare(const DirectoryState& directoryState, const std::string& path, const size_t fileDataHashSeed, std::vector<std::string>& missingDirectories, std::vector<std::string>& missingFiles, std::vector<std::string>& unequalFiles);
}
