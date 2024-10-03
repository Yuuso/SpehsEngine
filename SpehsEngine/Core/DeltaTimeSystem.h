#pragma once

#include "SpehsEngine/Core/SE_Time.h"


namespace se
{
	namespace time
	{
		class DeltaTimeSystem
		{
		public:
			DeltaTimeSystem(const std::string& debugName = "unnamed");
			virtual ~DeltaTimeSystem();

			void init();
			void update();

			//Public attributes for maximum speed
			float deltaSeconds = 0.0f;
			time::Time deltaTime;

		private:
			std::string name;
			time::Time deltaTimestamp;
		};
	}
}