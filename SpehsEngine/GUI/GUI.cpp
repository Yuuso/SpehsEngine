#include "SpehsEngine/GUI/GUI.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Rendering/Rendering.h"
#include "SpehsEngine/Audio/Audio.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"

namespace spehs
{
	namespace gui
	{
		namespace
		{
			bool initialized = false;
		}

		int initialize()
		{
			_ASSERT(spehs::core::isInitialized() && "Spehs core must be initialized");
			_ASSERT(spehs::input::isInitialized() && "Spehs input must be initialized");
			_ASSERT(spehs::rendering::isInitialized() && "Spehs rendering must be initialized");
			_ASSERT(spehs::audio::isInitialized() && "Spehs audio must be initialized");
			log::info("Current SpehsEngine gui library build: " + getVersion());

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