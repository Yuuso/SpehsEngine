#pragma once
#include <stdint.h>

/*
Forward declaration
IMPORTANT NOTE: This requires a modified version of the SDL_joystick.h where SDL_JoystickGUID is not typedefined from an anonymous struct
*/
struct SDL_JoystickGUID;
typedef struct SDL_JoystickGUID SDL_JoystickGUID;

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