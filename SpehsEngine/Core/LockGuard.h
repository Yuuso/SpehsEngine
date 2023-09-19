#pragma once

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/ScopeProfiler.h"

#if SE_CONFIGURATION == SE_CONFIGURATION_FINAL_RELEASE
#define SE_LOCK_GUARD_PROFILING_ENABLED SE_FALSE
#else
#define SE_LOCK_GUARD_PROFILING_ENABLED SE_FALSE
#endif


namespace se
{
	template<typename Mutex>
	class LockGuard
	{
	public:

		LockGuard(Mutex& _mutex)
#if SE_LOCK_GUARD_PROFILING_ENABLED == SE_TRUE
			: lockGuard((ScopeProfiler::enterSectionManually("", __FUNCTION__, __FILE__, __LINE__), _mutex))
#else
			: lockGuard(_mutex)
#endif
		{
#if SE_LOCK_GUARD_PROFILING_ENABLED == SE_TRUE
			ScopeProfiler::leaveSectionManually();
#endif
		}

#if SE_LOCK_GUARD_PROFILING_ENABLED == SE_TRUE
		/*
			NOTE: _incrementHoldLockTime reference must be valid for the duration of the LockGuard instance.
		*/
		LockGuard(Mutex& _mutex, time::Time& _incrementLockTime, time::Time& _incrementHoldLockTime)
			: timestamp(time::getProfilerTimestamp())
			, incrementHoldLockTime(&_incrementHoldLockTime)
			, lockGuard((ScopeProfiler::enterSectionManually("", __FUNCTION__, __FILE__, __LINE__), _mutex))
		{
			ScopeProfiler::leaveSectionManually();
			const time::Time postLockTimestamp = time::getProfilerTimestamp();
			const time::Time acquireLockDuration = postLockTimestamp - timestamp;
			_incrementLockTime += acquireLockDuration;
			timestamp = postLockTimestamp;
		}
#else
		LockGuard(Mutex& _mutex, time::Time& _incrementLockTime, time::Time& _incrementHoldLockTime)
			: lockGuard(_mutex)
		{
		}
#endif

#if SE_LOCK_GUARD_PROFILING_ENABLED == SE_TRUE
		~LockGuard()
		{
			if (incrementHoldLockTime)
			{
				const time::Time holdLockDuration = time::getProfilerTimestamp() - timestamp;
				*incrementHoldLockTime += holdLockDuration;
			}
		}
#endif

	private:

#if SE_LOCK_GUARD_PROFILING_ENABLED == SE_TRUE
		time::Time timestamp;
		time::Time* incrementHoldLockTime = nullptr;
#endif
		std::lock_guard<Mutex> lockGuard;

	};
}
