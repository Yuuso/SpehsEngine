#include "stdafx.h"
#include "SpehsEngine/Core/ScopedFrameLimiter.h"

#include "SpehsEngine/Core/ScopeProfiler.h"


namespace se
{
	namespace time
	{
		ScopedFrameLimiter::ScopedFrameLimiter(const Time scopedMinimumLifetime)
			: endTime(now() + scopedMinimumLifetime)
		{
		}
		ScopedFrameLimiter::~ScopedFrameLimiter()
		{
			SE_SCOPE_PROFILER();
			const Time remaining = endTime - now();
			if (remaining.value > 0)
			{
				sleep(remaining);
			}
		}
	}
}
