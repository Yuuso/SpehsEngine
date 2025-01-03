#pragma once


namespace se
{
	/* Records time at the time of allocation. Elapsed time can be retrieved using get() at any time. */
	class ScopeTimer
	{
	public:
		ScopeTimer();
		inline Time get() const { return getEpochTime() - time; }
	private:
		Time time;
	};
}
