#include "stdafx.h"
#include "SpehsEngine/Input/GUID.h"


namespace se
{
	GUID::GUID(const SDL_JoystickGUID& other)
	{
		for (int i = 0; i < 16; i++)
			guid[i] = other.data[i];
	}

	GUID::GUID(const SDL_JoystickGUID&& other)
	{
		for (int i = 0; i < 16; i++)
			guid[i] = other.data[i];
	}

	void GUID::operator=(GUID& other)
	{
		for (int i = 0; i < 16; i++)
			guid[i] = other.guid[i];
	}

	void GUID::operator=(GUID&& other)
	{
		for (int i = 0; i < 16; i++)
			guid[i] = other.guid[i];
	}

	void GUID::operator=(SDL_JoystickGUID& other)
	{
		for (int i = 0; i < 16; i++)
			guid[i] = other.data[i];
	}

	void GUID::operator=(SDL_JoystickGUID&& other)
	{
		for (int i = 0; i < 16; i++)
			guid[i] = other.data[i];
	}

	bool GUID::operator==(const GUID& other) const
	{
		for (int i = 0; i < 16; i++)
			if (guid[i] != other.guid[i])
				return false;
		return true;
	}

	bool GUID::operator==(const SDL_JoystickGUID& other) const
	{
		for (int i = 0; i < 16; i++)
			if (guid[i] != other.data[i])
				return false;
		return true;
	}

	bool GUID::operator!=(const GUID& other) const
	{
		for (int i = 0; i < 16; i++)
			if (guid[i] != other.guid[i])
				return true;
		return false;
	}

	uint8_t GUID::operator[](const int index) const
	{
		return guid[index];
	}
};