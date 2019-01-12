#pragma once
#include "SpehsEngine/Core/SE_Time.h"

namespace se
{
	namespace time
	{
		/* Records time at the time of allocation. Elapsed time can be retrieved using get() at any time. */
		class ScopeTimer
		{
		public:
			ScopeTimer();
			inline Time get() const { return now() - time; }
		private:
			Time time;
		};
	}
}