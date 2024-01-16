#include "stdafx.h"
#include "SpehsEngine/Graphics/Display.h"

#include "SDL/SDL.h"


namespace se::gfx
{
	Display::Display(int _index)
		: displayIndex(_index)
	{
		const int numDisplays = SDL_GetNumVideoDisplays();
		if (displayIndex < 0 || displayIndex >= numDisplays)
		{
			log::error("Invalid display index!");
		}
	}
	int Display::getIndex() const
	{
		return displayIndex;
	}
	const char* Display::getName() const
	{
		const char* name = SDL_GetDisplayName(displayIndex);
		if (!name)
		{
			logError("Failed to get display name!");
			return "Unknown display";
		}
		return name;
	}
	glm::ivec2 Display::getSize() const
	{
		SDL_DisplayMode displayMode;
		if (SDL_GetDesktopDisplayMode(displayIndex, &displayMode) != 0)
		{
			logError("Failed to get display size!");
			return glm::ivec2(0);
		}
		return glm::ivec2(displayMode.w, displayMode.h);
	}
	DisplayDPI Display::getDPI() const
	{
		DisplayDPI result;
		if (SDL_GetDisplayDPI(displayIndex, &result.diagonalDPI, &result.horizontalDPI, &result.verticalDPI) != 0)
		{
			logError("Failed to get display dpi!");
			return {};
		}
		return result;
	}
	std::vector<DisplayMode> Display::getModes() const
	{
		std::vector<DisplayMode> result;
		const int displayModeCount = SDL_GetNumDisplayModes(displayIndex);
		for (int i = 0; i < displayModeCount; i++)
		{
			SDL_DisplayMode sdlDisplayMode;
			if (SDL_GetDisplayMode(displayIndex, i, &sdlDisplayMode) != 0)
			{
				logError("Failed to get display mode " + std::to_string(i));
				continue;
			}

			bool duplicate = false;
			for (const DisplayMode& displayMode : result)
			{
				if (sdlDisplayMode.w == displayMode.width &&
					sdlDisplayMode.h == displayMode.height &&
					sdlDisplayMode.refresh_rate == displayMode.refreshRate)
				{
					duplicate = true;
					break;
				}
			}
			if (duplicate)
				continue;

			result.push_back(DisplayMode());
			result.back().width = sdlDisplayMode.w;
			result.back().height = sdlDisplayMode.h;
			result.back().refreshRate = sdlDisplayMode.refresh_rate;
		}
		return result;
	}
	int Display::getRefreshRate() const
	{
		SDL_DisplayMode displayMode;
		if (SDL_GetCurrentDisplayMode(displayIndex, &displayMode) != 0)
		{
			logError("Failed to get display refresh rate!");
			return 0;
		}
		return displayMode.refresh_rate;
	}
	void Display::logError(const std::string& _message) const
	{
		log::error(_message + " " + SDL_GetError());
	}
	std::vector<Display> getDisplayList()
	{
		const int numDisplays = SDL_GetNumVideoDisplays();
		if (numDisplays < 0)
		{
			log::error(std::string("Failed to get number of displays! ") + SDL_GetError());
			return {};
		}

		std::vector<Display> result;
		for (int i = 0; i < numDisplays; i++)
		{
			result.emplace_back(i);
		}
		return result;
	}
}
