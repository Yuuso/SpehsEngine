#pragma once

#include <string>
#include <vector>


namespace se
{
	struct DirectoryState
	{
		enum class Flag : uint8_t
		{
			None = 0,
			Read = 1 << 0,
			Hash = 1 << 1,
			Size = 1 << 2,
		};

		struct FileState
		{
			std::string path;
			std::vector<uint8_t> data;
			/* The file hash is a hash value that is computed using the file's local (DirectoryState) path and file data contents. */
			uint32_t fileHash = 0u;
			uint64_t fileSize = 0;
		};

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
	void getDirectoryState(DirectoryState& directoryState, const std::string& path, const DirectoryState::Flag flags, const uint32_t fileHashSeed);

	void getDirectoriesAndFiles(const DirectoryState& directoryState, std::vector<std::string>& directories, std::vector<std::string>& files);

	/*
		Compares given directory state against given path.
		Detects missing directories, files and unequal (data hash comparison) files.
		DirectoryState file hashes must be pre-calculated.
	*/
	void compare(const DirectoryState& directoryState, const std::string& path, const uint32_t fileHashSeed, std::vector<std::string>& missingDirectories, std::vector<std::string>& missingFiles, std::vector<std::string>& unequalFiles);
}

template<> template<typename S, typename T>
static bool se::Serial<se::DirectoryState::FileState>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.path, "path");
	se_serial(_serial, _value.fileHash, "fileHash");
	se_serial(_serial, _value.fileSize, "fileSize");
	// NOTE: not data...
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::DirectoryState>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.path, "path");
	se_serial(_serial, _value.files, "files");
	se_serial(_serial, _value.directories, "directories");
	return true;
}
