#pragma once
#include <stdint.h>

/*
Forward declaration
IMPORTANT NOTE: This requires a modified version of the SDL_joystick.h where SDL_JoystickGUID is not typedefined from an anonymous struct
*/
struct SDL_JoystickGUID;
typedef struct SDL_JoystickGUID SDL_JoystickGUID;

namespace spehs
{
	struct GUID
	{
		GUID();
		GUID(const SDL_JoystickGUID& other);
		void operator=(GUID& other);
		void operator=(SDL_JoystickGUID& other);
		bool operator==(GUID& other);
		bool operator==(SDL_JoystickGUID& other);
		bool operator!=(GUID& other);
		uint8_t operator[](int index);
		uint8_t guid[16];
	};
}