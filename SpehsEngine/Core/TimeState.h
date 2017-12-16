#pragma once
#include "SpehsEngine/Core/Time.h"

namespace spehs
{
	namespace time
	{
		class TimeState
		{
		public:
			TimeState(const std::string& debugName = "unnamed");
			virtual ~TimeState();
			void initialize();
			void update();

			float deltaSeconds;
			spehs::time::Time deltaTime;
			spehs::time::Time runTime;

		private:
			std::string name;
			spehs::time::Time updateTimestamp;
		};
	}
}