#pragma once
#include "SpehsEngine/Core/SE_Time.h"

namespace se
{
	namespace time
	{
		/* get() returns time since last get(), or time since allocation if get() hasn't been used. */
		class LapTimer
		{
		public:
			LapTimer();
			Time get();
		private:
			Time time;
		};
	}
}