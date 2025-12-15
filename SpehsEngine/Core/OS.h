#pragma once


namespace se
{
	struct ProcessHandle
	{
		static uint64_t invalidValue;
		explicit operator bool() const { return value != invalidValue; }
		bool isValid() const { return value != invalidValue; }
		uint64_t value = invalidValue;
	};

	/* Copies data into operating system clipboard. Returns true upon success */
	bool copyToClipBoard(const void* data, size_t bytes);

	/* Copy data from the operating system clipboard. Returns data copied in the string container. */
	std::string copyFromClipBoard();

	/* Creates a new process by launching the specified file. */
	ProcessHandle createProcess(const std::string& filename, const std::string& commandLine);
	bool terminateProcess(const ProcessHandle _processHandle);

	/* Calls function for each running process name. */
	bool forEachProcessName(const std::function<void(const std::string_view& processName)>& callback);

	/* Get the current directory. Contains a trailing backslash. */
	const std::wstring& getCurrentDirectory();

	/* Get the appdata directory for the current user. Contains a trailing backslash. */
	const std::wstring& getUserDataDirectory();

	/* Get temporary cache directory. Contains a trailing backslash. */
	const std::string& getTempDirectory();

	void testCrash();
}