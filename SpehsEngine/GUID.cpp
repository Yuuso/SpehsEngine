#include "GUID.h"
#include <SDL/SDL_joystick.h>


namespace spehs
{
	GUID::GUID()
	{}
	GUID::GUID(const SDL_JoystickGUID& other)
	{
		for (int i = 0; i < 16; i++)
			guid[i] = other.data[i];
	}
	void GUID::operator = (GUID& other)
	{//Assignent 
		for (int i = 0; i < 16; i++)
			guid[i] = other.guid[i];
	}
	void GUID::operator=(SDL_JoystickGUID& other)
	{//Assignment
		for (int i = 0; i < 16; i++)
			guid[i] = other.data[i];
	}
	bool GUID::operator==(GUID& other)
	{
		for (int i = 0; i < 16; i++)
			if (guid[i] != other.guid[i])
				return false;
		return true;
	}
	bool GUID::operator==(SDL_JoystickGUID& other)
	{
		for (int i = 0; i < 16; i++)
			if (guid[i] != other.data[i])
				return false;
		return true;
	}
	bool GUID::operator!=(GUID& other)
	{
		for (int i = 0; i < 16; i++)
			if (guid[i] != other.guid[i])
				return true;
		return false;
	}
	uint8_t GUID::operator[](int index)
	{
		return guid[index];
	}
};