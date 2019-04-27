#pragma once
#include "SpehsEngine/Core/SE_Time.h"

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
