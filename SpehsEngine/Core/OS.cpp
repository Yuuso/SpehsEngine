#include "SpehsEngine/Core/OS.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Windows.h>
#else
#endif



namespace se
{
	bool copyToClipBoard(const void* data, size_t bytes)
	{
#if false && (defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__))

		if (!OpenClipboard(0))
			return false;

		if (!EmptyClipboard())
		{
			while (!CloseClipboard()) {}
			return false;
		}

		//Allocate global memory block and copy data
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, bytes);
		if (!hg)
		{
			while (!CloseClipboard()) {}
			return false;
		}
		memcpy(GlobalLock(hg), data, bytes);
		GlobalUnlock(hg);

		//Set clipboard data
		bool success = SetClipboardData(CF_TEXT, hg);

		//Close and return
		while (!CloseClipboard()) {}
		GlobalFree(hg);
		return success;
#else
		return false;
#endif
	}
	std::string copyFromClipBoard()
	{
#if false && (defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__))
		std::string data;

		if (!OpenClipboard(0))
			return data;

		HANDLE h = GetClipboardData(CF_TEXT);
		if (h)
		{
			size_t length(strlen((char*)h));
			data.resize(length);
			memcpy(&data[0], h, length);
		}

		while (!CloseClipboard()) {}
		return data;
#else
		return "";
#endif
	}
}