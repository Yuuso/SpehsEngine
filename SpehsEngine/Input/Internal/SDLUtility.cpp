#include "stdafx.h"
#include "SpehsEngine/Input/Internal/SDLUtility.h"


namespace se
{
	namespace input
	{
		GUID fromSdlGuid(const SDL_GUID& sdlGuid)
		{
			GUID guid;
			for (int i = 0; i < 16; i++)
				guid.guid[i] = sdlGuid.data[i];
			return guid;
		}
	}
}
