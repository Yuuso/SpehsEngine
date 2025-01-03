#include "stdafx.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"


namespace se
{
	DeltaTimeSystem::DeltaTimeSystem(const std::string& debugName)
		: name(debugName)
	{
		init();
	}

	void DeltaTimeSystem::init()
	{
		deltaSeconds = 0.0f;
		deltaTime.value = 0;
		deltaTimestamp = getEpochTime();
	}

	void DeltaTimeSystem::update()
	{
		const Time now = getEpochTime();
		deltaTime = now - deltaTimestamp;
		deltaTimestamp = now;
		deltaSeconds = deltaTime.asSeconds();
	}
}
