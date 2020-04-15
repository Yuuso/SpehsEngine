#include "stdafx.h"
#include "SpehsEngine/Core/OS.h"
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace
{
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
	std::string getLastErrorMessage()
	{
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		return std::string((const char*)lpMsgBuf);
	}
#endif
}

namespace se
{
	bool copyToClipBoard(const void* data, size_t bytes)
	{
#if SE_PLATFORM == SE_PLATFORM_WINDOWS

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
		HANDLE success = SetClipboardData(CF_TEXT, hg);

		//Close and return
		while (!CloseClipboard()) {}
		GlobalFree(hg);
		if (success)
		{
			return true;
		}
		else
		{
			return false;
		}
#else
		return false;
#endif
	}
	std::string copyFromClipBoard()
	{
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
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

	bool createProcess(const std::string& filename, const std::string& commandLine)
	{
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
		std::string commandLineCopy = commandLine;

		// additional information
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION processInfo;

		// set the size of the structures
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		ZeroMemory(&processInfo, sizeof(processInfo));

		// start the program up
		auto result = CreateProcess(
			filename.c_str(),				// the path
			&commandLineCopy[0],			// Command line
			NULL,							// Process handle not inheritable
			NULL,							// Thread handle not inheritable
			FALSE,							// Set handle inheritance to FALSE
			0,								// No creation flags
			NULL,							// Use parent's environment block
			NULL,							// Use parent's starting directory 
			&startupInfo,					// Pointer to STARTUPINFO structure
			&processInfo					// Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);
		if (result == 0)
		{
			se::log::error("Failed to create process '" + filename + "'. Error: " + getLastErrorMessage());
			return false;
		}
		else
		{
			return true;
		}
#else
		static_assert(false && "Create process implementation is missing.");
		return false;
#endif
	}
}