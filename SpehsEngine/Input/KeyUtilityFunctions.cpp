#include "stdafx.h"
#include "SpehsEngine/Input/KeyUtilityFunctions.h"

#include "SDL/SDL.h"

//se_pragma_optimize(off);

namespace se
{
	namespace input
	{
		bool isKeyDown(const Key key)
		{
			int numkeys = 0;
			const Uint8 *keyboardState = SDL_GetKeyboardState(&numkeys);
			const int scancode = SDL_GetScancodeFromKey(SDL_Keycode(key));
			if (scancode >= numkeys)
			{
				se_assert(false && "key value is not supported");
				return false;
			}
			return keyboardState[scancode] != 0;
		}

		bool isCtrlDown()
		{
			const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
			return keyboardState[SDL_Scancode::SDL_SCANCODE_LCTRL] != 0 || keyboardState[SDL_Scancode::SDL_SCANCODE_RCTRL] != 0;
		}

		bool isShiftDown()
		{
			const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
			return keyboardState[SDL_Scancode::SDL_SCANCODE_LSHIFT] != 0 || keyboardState[SDL_Scancode::SDL_SCANCODE_RSHIFT] != 0;
		}
	}
}
