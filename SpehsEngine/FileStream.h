#pragma once
#include <string>
#include <fstream>
namespace spehs
{
	/**Writes string to stream*/
	void writeString(std::string& string, std::ofstream* stream);
	/**Reads string from stream, written in writeString() format*/
	void readString(std::string& string, std::ifstream* stream);
	/**Returns true if directory exists at given path*/
	bool directoryExists(std::string& path);
	/**Returns true if file exists at given path*/
	bool fileExists(std::string& path);
	/**Returns false if failed to create directory at given path*/
	bool createDirectory(std::string& path);
	/**If directory doesn't exist, creates one. If creation fails, returns false*/
	bool verifyDirectory(std::string& path);
}