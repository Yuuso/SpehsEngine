#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"

#include "SpehsEngine/Core/Log.h"
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
										 SDL_WINDOWPOS_UNDEFINED,
										 SDL_WINDOWPOS_UNDEFINED,
										 window.getWidth(),
										 window.getHeight(),
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
				frameBufferHandle = bgfx::createFrameBuffer(wmi.info.win.window, window.getWidth(), window.getHeight());
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
			_renderContext.currentWindow = this;
			for (auto& view : window.views)
			{
				if (bgfx::isValid(frameBufferHandle))
				{
					bgfx::setViewFrameBuffer(_renderContext.currentViewId, frameBufferHandle);
				}
				view->render(_renderContext);
			}
		}

		float WindowInstance::getWidth() const
		{
			return (float)window.getWidth();
		}
		float WindowInstance::getHeight() const
		{
			return (float)window.getHeight();
		}
	}
}