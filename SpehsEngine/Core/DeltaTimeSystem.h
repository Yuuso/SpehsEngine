#pragma once
#include "SpehsEngine/Core/Time.h"
#include <string>

namespace se
{
	namespace time
	{
		class DeltaTimeSystem
		{
		public:
			DeltaTimeSystem(const std::string& debugName = "unnamed");
			virtual ~DeltaTimeSystem();

			void deltaTimeSystemInitialize();
			void deltaTimeSystemUpdate();

			//Public attributes for maximum speed
			float deltaSeconds = 0.0f;
			se::time::Time deltaTime;

		private:
			std::string name;
			se::time::Time deltaTimestamp;
		};
	}
}