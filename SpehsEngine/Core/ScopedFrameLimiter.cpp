#include "stdafx.h"
#include "SpehsEngine/Core/ScopedFrameLimiter.h"

#include "SpehsEngine/Core/ScopeProfiler.h"


namespace se
{
	ScopedFrameLimiter::ScopedFrameLimiter(const Time scopedMinimumLifetime)
		: endTime(getEpochTime() + scopedMinimumLifetime)
	{
	}
	ScopedFrameLimiter::~ScopedFrameLimiter()
	{
		SE_SCOPE_PROFILER();
		const Time remaining = endTime - getEpochTime();
		if (remaining.value > 0)
		{
			sleep(remaining);
		}
	}
}
