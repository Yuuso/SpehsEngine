#pragma once

#include "SpehsEngine/Core/SE_Time.h"


namespace se
{
	class DeltaTimeSystem
	{
	public:
		DeltaTimeSystem(const std::string& debugName = "unnamed");
		virtual ~DeltaTimeSystem() = default;

		void init();
		void update();

		float deltaSeconds = 0.0f;
		Time deltaTime;

	private:
		std::string name;
		Time deltaTimestamp;
	};
}