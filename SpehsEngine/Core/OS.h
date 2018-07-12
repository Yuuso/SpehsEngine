#pragma once
#include <string>
#include <utility>

namespace se
{
	/**Copies data into operating system clipboard. Returns true upon success*/
	bool copyToClipBoard(const void* data, size_t bytes);

	/**Copy data from the operating system clipboard. Returns data copied in the string container.*/
	std::string copyFromClipBoard();
}