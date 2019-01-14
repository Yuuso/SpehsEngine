#pragma once
#include <string>
#include <vector>

namespace se
{
	/* Returns true if file exists at given path. File may be a directory. */
	bool fileExists(const std::string& path);
	bool isDirectory(const std::string& path);

	/* Returns true if file exists and is removed succesfully. */
	bool removeFile(const std::string& path);

	/* Returns true if file exists and is renamed successfully. */
	bool renameFile(const std::string& oldPath, const std::string& newPath);

	/* Returns false if directory did not exist and failed to create directory at the given path. */
	bool createDirectory(const std::string& path);

	/* Returns false if directory did not exist and couldn't create one. */
	bool verifyDirectory(const std::string& path);
		
	/**Returns a vector of strings, each string representing a file found in the given directory.
	Directory path is not included in the file string. If file type is specified, the file type will not be included in the file string.
	If file type is specified, only files of that type are returned.*/
	std::vector<std::string> listFilesInDirectory(const std::string& directoryPath, std::string fileType = "");

	/**Returns a vector of strings, each string representing a sub directory found in the given directory.
	Directory path is not included in the sub directory string.*/
	std::vector<std::string> listSubDirectoriesInDirectory(const std::string& directoryPath);

	/*
		Note: The current path for many operating systems is a dangerous global state.
		It may be changed unexpectedly by a third-party or system library functions, or by another thread.
	*/
	std::string getCurrentPath();
}
