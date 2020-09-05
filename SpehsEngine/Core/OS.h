#pragma once
#include <string>
#include <vector>

namespace se
{
	/* Copies data into operating system clipboard. Returns true upon success */
	bool copyToClipBoard(const void* data, size_t bytes);

	/* Copy data from the operating system clipboard. Returns data copied in the string container. */
	std::string copyFromClipBoard();

	/* Creates a new process by launching the specified file. */
	bool createProcess(const std::string& filename, const std::string& commandLine);

	/* Calls function for each running process name. */
	bool forEachProcessName(const std::function<void(const std::string_view& processName)>& callback);
}