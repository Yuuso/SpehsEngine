#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"

#include <Windows.h>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"

#pragma comment(lib, "psapi.lib")


namespace se
{
	namespace graphics
	{
		WindowInstance::WindowInstance(const Window& _window, const bool _isDefault)
			: window(_window)
			, isDefault(_isDefault)
		{
			sdlWindow = SDL_CreateWindow("SpehsEngine Window",
										 SDL_WINDOWPOS_CENTERED,
										 SDL_WINDOWPOS_CENTERED,
										 window.widthGet(),
										 window.widthGet(),
										 SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

			SDL_SysWMinfo wmi;
			SDL_VERSION(&wmi.version);
			if (SDL_GetWindowWMInfo(sdlWindow, &wmi) != SDL_TRUE)
			{
				std::string error = "Unable to get SDL window info: ";
				error += SDL_GetError();
				se::log::error(error);
				return;
			}

			if (isDefault)
			{
				bgfx::PlatformData platformData;

				// Windows
				platformData.ndt = NULL;
				//

				platformData.nwh = (void*)wmi.info.win.window;

				platformData.context = NULL;
				platformData.backBuffer = NULL;
				platformData.backBufferDS = NULL;
				bgfx::setPlatformData(platformData);
			}
			else
			{
				frameBufferHandle = bgfx::createFrameBuffer(wmi.info.win.window, window.widthGet(), window.widthGet());
			}
		}
		WindowInstance::~WindowInstance()
		{
			SDL_DestroyWindow(sdlWindow);
		}

		bool WindowInstance::operator==(const Window& _other)
		{
			return &window == &_other;
		}

		void WindowInstance::render(RenderContext& _renderContext)
		{
			for (auto& view : window.views)
			{
				if (bgfx::isValid(frameBufferHandle))
				{
					bgfx::setViewFrameBuffer(_renderContext.currentViewId, frameBufferHandle);
					bgfx::setViewClear(_renderContext.currentViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x101010ff, 1.0f, 0);
				}
				view->render(_renderContext);
			}
		}
	}
}