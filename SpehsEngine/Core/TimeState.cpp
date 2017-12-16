#include "SpehsEngine/Core/TimeState.h"

namespace spehs
{
	namespace time
	{
		TimeState::TimeState(const std::string& debugName)
			: deltaSeconds(0.0f)
			, deltaTime(0)
			, name(debugName)
			, updateTimestamp(now())
		{

		}

		TimeState::~TimeState()
		{

		}

		void TimeState::initialize()
		{
			deltaSeconds = 0.0f;
			deltaTime.value = 0;
			updateTimestamp = now();
		}

		void TimeState::update()
		{
			const spehs::time::Time now = spehs::time::now();
			deltaTime = now - updateTimestamp;
			deltaSeconds = deltaTime.asSeconds();
			updateTimestamp = now;
		}
	}
}