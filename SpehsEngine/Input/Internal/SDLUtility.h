#pragma once

#include "SpehsEngine/Input/GUID.h"
#include "SDL/SDL_guid.h"


namespace se
{
	namespace input
	{
		GUID fromSdlGuid(const SDL_GUID& sdlGuid);
	}
}
