#include "stdafx.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"

namespace se
{
	namespace time
	{
		DeltaTimeSystem::DeltaTimeSystem(const std::string& debugName)
			: name(debugName)
		{

		}

		DeltaTimeSystem::~DeltaTimeSystem()
		{

		}

		void DeltaTimeSystem::deltaTimeSystemInitialize()
		{
			deltaSeconds = 0.0f;
			deltaTime.value = 0;
			deltaTimestamp = now();
		}

		void DeltaTimeSystem::deltaTimeSystemUpdate()
		{
			const se::time::Time now = se::time::now();
			deltaTime = now - deltaTimestamp;
			deltaTimestamp = now;
			deltaSeconds = deltaTime.asSeconds();
		}
	}
}
