#pragma once


namespace se
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
