#include "stdafx.h"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"

#include "SDL/SDL_mouse.h"

namespace se
{
	namespace input
	{
		glm::ivec2 getMousePosition()
		{
			if (SDL_Window* const focusedWindow = SDL_GetMouseFocus())
			{
				glm::ivec2 position;
				SDL_GetMouseState(&position.x, &position.y);
				int height = 0;
				SDL_GetWindowSize(focusedWindow, nullptr, &height);
				position.y = height - position.y;
				return position;
			}
			else
			{
				return glm::ivec2();
			}
		}

		glm::vec2 getMousePositionf()
		{
			if (SDL_Window* const focusedWindow = SDL_GetMouseFocus())
			{
				glm::ivec2 position;
				SDL_GetMouseState(&position.x, &position.y);
				int height = 0;
				SDL_GetWindowSize(focusedWindow, nullptr, &height);
				position.y = height - position.y;
				return glm::vec2(position.x, position.y);
			}
			else
			{
				return glm::vec2();
			}
		}

		bool isMouseButtonDown(const MouseButton mouseButton)
		{
			return (SDL_GetMouseState(nullptr, nullptr) & (1 << (Uint32(mouseButton) - 1))) != 0;
		}

		bool setRelativeMouseMode(const bool enabled)
		{
			if (getRelativeMouseMode() == enabled)
			{
				return true;
			}
			else
			{
				if (enabled)
				{
					return SDL_SetRelativeMouseMode(SDL_TRUE) == 0;
				}
				else
				{
					return SDL_SetRelativeMouseMode(SDL_FALSE) == 0;
				}
			}
		}

		bool getRelativeMouseMode()
		{
			return SDL_GetRelativeMouseMode() == SDL_TRUE;
		}

		bool setShowCursor(const bool enabled)
		{
			if (getShowCursor() == enabled)
			{
				return true;
			}
			else
			{
				if (enabled)
				{
					return SDL_ShowCursor(1) == 0;
				}
				else
				{
					return SDL_ShowCursor(0) == 0;
				}
			}
		}

		bool getShowCursor()
		{
			return SDL_ShowCursor(-1) == SDL_TRUE;
		}
	}
}
