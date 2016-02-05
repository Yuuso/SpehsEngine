#pragma once
#include <SDL_joystick.h>

struct GUID
{
	GUID(){}
	GUID(const SDL_JoystickGUID& other)
	{
		for (int i = 0; i < 16; i++)
			guid.data[i] = other.data[i];
	}
	void operator=(GUID& other)
	{//Assignent 
		for (int i = 0; i < 16; i++)
			guid.data[i] = other.guid.data[i];
	}
	void operator=(SDL_JoystickGUID& other)
	{//Assignment
		for (int i = 0; i < 16; i++)
			guid.data[i] = other.data[i];
	}
	bool operator==(GUID& other)
	{
		for (int i = 0; i < 16; i++)
			if (guid.data[i] != other.guid.data[i])
				return false;
		return true;
	}
	bool operator==(SDL_JoystickGUID& other)
	{
		for (int i = 0; i < 16; i++)
			if (guid.data[i] != other.data[i])
				return false;
		return true;
	}
	bool operator!=(GUID& other)
	{
		for (int i = 0; i < 16; i++)
			if (guid.data[i] != other.guid.data[i])
				return true;
		return false;
	}
	Uint8 operator[](int index)
	{
		return guid.data[index];
	}
	SDL_JoystickGUID guid;
};