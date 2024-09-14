#pragma once


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
