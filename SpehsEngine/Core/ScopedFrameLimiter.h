#pragma once

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/ScopeProfiler.h"


namespace se
{
	namespace time
	{
		class ScopedFrameLimiter
		{
		public:
			ScopedFrameLimiter(const Time scopedMinimumLifetime)
				: endTime(now() + scopedMinimumLifetime)
			{
			}
			~ScopedFrameLimiter()
			{
				SE_SCOPE_PROFILER();
				const Time remaining = endTime - now();
				if (remaining.value > 0)
				{
					sleep(remaining);
				}
			}
		private:
			const Time endTime;
		};
	}
}
