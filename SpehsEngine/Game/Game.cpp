#include "SpehsEngine/Game/Game.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"

namespace spehs
{
	namespace game
	{
		namespace
		{
			bool initialized = false;
		}

		int initialize(const std::string& _windowName)
		{
			log::info("Current SpehsEngine game library build: " + getVersion());

			initialized = true;
			return 0;
		}
		
		void uninitialize()
		{
			initialized = false;
		}

		bool isInitialized()
		{
			return initialized;
		}

		std::string getVersion()
		{
			return std::string("0");
		}
	}
}