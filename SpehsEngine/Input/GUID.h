#pragma once

#include "SDL/SDL_joystick.h"
#include <stdint.h>


// TODO: SDL include should not be needed for the SpehsEngine API!

namespace se
{
	struct GUID
	{
		GUID() = default;
		GUID(const SDL_JoystickGUID& other);
		GUID(const SDL_JoystickGUID&& other);
		void operator=(GUID& other);
		void operator=(SDL_JoystickGUID& other);
		void operator=(GUID&& other);
		void operator=(SDL_JoystickGUID&& other);
		bool operator==(const GUID& other) const;
		bool operator==(const SDL_JoystickGUID& other) const;
		bool operator!=(const GUID& other) const;
		uint8_t operator[](const int index) const;
		uint8_t guid[16];
	};
}