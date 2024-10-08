#include "stdafx.h"
#include "SpehsEngine/Input/Joystick.h"

#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/Internal/SDLUtility.h"
#include "SpehsEngine/Input/InputEnumerations.h"
#include "SDL/SDL.h"
#include "SDL/SDL_joystick.h"
#include "SDL/SDL_events.h"


namespace se
{
	namespace input
	{
		GUID getJoystickDeviceGUID(int _deviceIndex)
		{
			return fromSdlGuid(SDL_JoystickGetDeviceGUID(_deviceIndex));
		}

		Joystick::Joystick(InputManager& _inputManager, int index)
			: inputManager(_inputManager)
		{
			joystick = SDL_JoystickOpen(index);
			if (joystick == NULL)
			{
				std::string error = "Failed to open SDL_joystick! ";
				error += SDL_GetError();
				log::error(error);
				return;
			}

			ID = SDL_JoystickInstanceID(joystick);
			name = SDL_JoystickNameForIndex(index);
			guid = fromSdlGuid(SDL_JoystickGetDeviceGUID(index));
			buttonCount = SDL_JoystickNumButtons(joystick);
			axisCount = SDL_JoystickNumAxes(joystick);
			hatCount = SDL_JoystickNumHats(joystick);
			offline = false;
			std::string str("Joystick created: ");
			str += SDL_JoystickName(joystick);
			log::info(str);
		}

		Joystick::~Joystick()
		{
			goOffline();
		}

		void Joystick::goOffline()
		{
			if (joystick != NULL)
				SDL_JoystickClose(joystick);
			joystick = nullptr;
			offline = true;
			log::info(name + " went offline!");
		}

		void Joystick::goOnline(SDL_Joystick* newJs)
		{
			joystick = newJs;
			offline = false;

			if (!SDL_JoystickGetAttached(newJs))
				log::info(name + " couldn't go online!");
			else
				log::info(name + " went online!");
		}

		float Joystick::getAxisState(int axisIndex)
		{
			if (offline)
				return 0.0f;
			return float(SDL_JoystickGetAxis(joystick, axisIndex)) / std::numeric_limits<int16_t>::max();
		}

		bool Joystick::isButtonDown(int buttonIndex)
		{
			if (offline)
				return false;
			if (SDL_JoystickGetButton(joystick, buttonIndex))
				return true;
			return false;
		}

		bool Joystick::isHatInPosition(int hatIndex, uint8_t position)
		{
			if (offline)
				return false;
			if (SDL_JoystickGetHat(joystick, hatIndex) == position)
				return true;
			return false;
		}

		int32_t Joystick::queryButtonDown()
		{
			if (offline)
				return -1;
			for (int i = 0; i < buttonCount; i++)
			{
				if (SDL_JoystickGetButton(joystick, i))
					return i;
			}
			return -1;
		}

		unsigned Joystick::getGUIDIndex()
		{
			unsigned index = 0;
			for (unsigned i = 0; i < inputManager.joysticks.size(); i++)
			{
				if (inputManager.joysticks[i] == this)
					return index;
				else if (inputManager.joysticks[i]->guid == guid)
					index++;//Found same guid among joysticks before this
			}
			return ~0u;
		}
	}
}