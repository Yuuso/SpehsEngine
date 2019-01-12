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
		If fileDataHashSeed is set to non-zero, each found file will be opened and data hash will be computed.
	*/
	void getDirectoryState(DirectoryState& directory, const std::string& path, const size_t fileDataHashSeed = 0);
}
