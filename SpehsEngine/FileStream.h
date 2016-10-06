#pragma once
#include <string>
#include <fstream>
#include <vector>
namespace spehs
{
	/**Writes string to stream*/
	void writeString(std::string& string, std::ofstream* stream);
	/**Writes string to buffer. Returns number of bytes read.*/
	size_t writeString(std::string& string, unsigned char* buffer);

	/**Reads string from stream, written in writeString() format*/
	void readString(std::string& string, std::ifstream* stream);
	/**Reads string from buffer, written in writeString() format. Returns number of bytes read.*/
	size_t readString(std::string& string, unsigned char* buffer);

	/**Returns true if directory exists at given path*/
	bool directoryExists(std::string& path);

	/**Returns true if file exists at given path*/
	bool fileExists(std::string& path);

	/**Returns true if file exists and was removed succesfully*/
	bool removeFile(std::string& path);

	/**Returns false if failed to create directory at given path*/
	bool createDirectory(std::string& path);

	/**If directory doesn't exist, creates one. If creation fails, returns false*/
	bool verifyDirectory(std::string& path);

	/**Returns a vector of strings, each string representing a file found in the given directory.
	Directory path is not included in the file string. If file type is specified, the file type will not be included in the file string.
	If file type is specified, only files of that type are returned.*/
	std::vector<std::string> listFilesInDirectory(std::string directoryPath, std::string fileType = "");
}