#include "stdafx.h"
#include "SpehsEngine/Core/Thread.h"

#include <thread>

#if SE_PLATFORM == SE_PLATFORM_WINDOWS
#include <windows.h>
#endif


namespace se
{
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)

	void setThreadName(uint32_t dwThreadID, const std::string& _name)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = _name.c_str();
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
		__try
		{
			static const DWORD MS_VC_EXCEPTION = 0x406D1388;
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
	void setThreadName(const std::string_view _name)
	{
		const std::string name(_name);
		setThreadName(GetCurrentThreadId(), name);
	}
	void setThreadName(std::thread& _thread, const std::string_view _name)
	{
		const std::string name(_name);
		DWORD threadId = ::GetThreadId(static_cast<HANDLE>(_thread.native_handle()));
		setThreadName(threadId, name);
	}
#else
	void setThreadName(std::thread& _thread, const std::string_view _name)
	{
		auto handle = _thread.native_handle();
		const std::string name(_name);
		pthread_setname_np(handle, name.c_str());
	}
#include <sys/prctl.h>
	void setThreadName(const std::string_view _name)
	{
		const std::string name(_name);
		prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
	}
#endif
}
