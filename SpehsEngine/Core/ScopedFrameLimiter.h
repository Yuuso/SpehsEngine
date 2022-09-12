#pragma once

#include "SpehsEngine/Core/SE_Time.h"


namespace se
{
	namespace time
	{
		class ScopedFrameLimiter
		{
		public:
			ScopedFrameLimiter(const Time scopedMinimumLifetime);
			~ScopedFrameLimiter();
		private:
			const Time endTime;
		};
	}
}
