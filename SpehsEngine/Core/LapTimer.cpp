#include "stdafx.h"
#include "SpehsEngine/Core/LapTimer.h"

namespace se
{
	namespace time
	{
		LapTimer::LapTimer()
			: time(now())
		{

		}

		Time LapTimer::get()
		{
			const Time n = now();
			const Time lapTime = now() - time;
			time = n;
			return lapTime;
		}
	}
}
