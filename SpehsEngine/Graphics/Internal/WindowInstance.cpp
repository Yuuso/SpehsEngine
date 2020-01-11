#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Assert.h"
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
		WindowInstance::WindowInstance(Window& _window, const bool _isDefault)
			: window(&_window)
			, isDefault(_isDefault)
		{
			windowDestroyedConnection = window->destroyedSignal.connect(boost::bind(&WindowInstance::windowDestroyed, this));

			sdlWindow = SDL_CreateWindow("SpehsEngine Window",
										 SDL_WINDOWPOS_UNDEFINED,
										 SDL_WINDOWPOS_UNDEFINED,
										 window->getWidth(),
										 window->getHeight(),
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
				frameBufferHandle = bgfx::createFrameBuffer(wmi.info.win.window, window->getWidth(), window->getHeight());
			}
		}
		WindowInstance::~WindowInstance()
		{
			if (bgfx::isValid(frameBufferHandle))
			{
				bgfx::destroy(frameBufferHandle);
				frameBufferHandle = BGFX_INVALID_HANDLE;
			}
			SDL_DestroyWindow(sdlWindow);
		}

		void WindowInstance::windowDestroyed()
		{
			window = nullptr;
		}

		bool WindowInstance::operator==(const Window& _other) const
		{
			return window == &_other;
		}

		bool WindowInstance::render(RenderContext& _renderContext)
		{
			if (!window)
				return false;

			_renderContext.currentWindow = this;
			for (size_t i = 0; i < window->views.size(); )
			{
				if (bgfx::isValid(frameBufferHandle))
					bgfx::setViewFrameBuffer(_renderContext.currentViewId, frameBufferHandle);
				if (!window->views[i]->render(_renderContext))
				{
					window->views[i].reset(window->views.back().release());
					window->views.pop_back();
					continue;
				}
				i++;
			}
			return true;
		}

		bool WindowInstance::wasDestroyed() const
		{
			return window == nullptr;
		}

		float WindowInstance::getWidth() const
		{
			se_assert(window);
			return (float)window->getWidth();
		}
		float WindowInstance::getHeight() const
		{
			se_assert(window);
			return (float)window->getHeight();
		}
	}
}