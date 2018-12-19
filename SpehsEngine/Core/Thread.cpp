#include "stdafx.h"
#include "SpehsEngine/Core/Thread.h"
#include <thread>
#ifdef _WIN32
#include <windows.h>
#endif



namespace se
{
#ifdef _WIN32
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)	
	void setThreadName(uint32_t dwThreadID, const char* threadName)
	{

		// DWORD dwThreadID = ::GetThreadId( static_cast<HANDLE>( t.native_handle() ) );

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
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
	void setThreadName(const std::string& name)
	{
		setThreadName(GetCurrentThreadId(), name.c_str());
	}
	void setThreadName(std::thread& thread, const std::string& name)
	{
		DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread.native_handle()));
		setThreadName(threadId, name.c_str());
	}
#else
	void setThreadName(std::thread& thread, const std::string& name)
	{
		auto handle = thread.native_handle();
		pthread_setname_np(handle, name.c_str());
	}
#include <sys/prctl.h>
	void setThreadName(const std::string& name)
	{
		prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
	}
#endif
}
