#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Assert.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#include "SDL/SDL_events.h"
#include "SDL/SDL_video.h"

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

			uint32_t windowFlags = 0;
			if (!window->isShown())
				enableBit(windowFlags, SDL_WINDOW_HIDDEN);
			if (window->getBorderless())
				enableBit(windowFlags, SDL_WINDOW_BORDERLESS);
			if (window->getResizable())
				enableBit(windowFlags, SDL_WINDOW_RESIZABLE);
			if (window->getMinimized())
				enableBit(windowFlags, SDL_WINDOW_MINIMIZED);
			if (window->getMaximized())
				enableBit(windowFlags, SDL_WINDOW_MAXIMIZED);
			if (window->getKeyboardFocus())
				enableBit(windowFlags, SDL_WINDOW_INPUT_FOCUS);
			if (window->getMouseFocus())
				enableBit(windowFlags, SDL_WINDOW_MOUSE_FOCUS);
			if (window->getConfinedInput())
				enableBit(windowFlags, SDL_WINDOW_INPUT_GRABBED);
			//SDL_WINDOW_ALLOW_HIGHDPI

			const int x = window->getX() != -1 ? window->getX() : SDL_WINDOWPOS_UNDEFINED;
			const int y = window->getY() != -1 ? window->getY() : SDL_WINDOWPOS_UNDEFINED;

			sdlWindow = SDL_CreateWindow(window->getName().c_str(),
										 x,
										 y,
										 window->getWidth(),
										 window->getHeight(),
										 windowFlags);

			if (window->getX() == -1 || window->getY() == -1)
			{
				int windowX, windowY;
				SDL_GetWindowPosition(sdlWindow, &windowX, &windowY);
				window->x = windowX;
				window->y = windowY;
			}
			if (window->getOpacity() != 1.0f)
			{
				SDL_SetWindowOpacity(sdlWindow, window->getOpacity());
			}
			window->updateFlags = 0;

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

#				if defined(_WIN32)
					platformData.ndt = NULL;
#				else
#					error Window handling not implemented!
#				endif

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

		void WindowInstance::render(RenderContext& _renderContext)
		{
			se_assert(window);
			if (window->getMinimized() || !window->isShown())
			{
				for (size_t i = 0; i < window->views.size(); )
				{
					if (window->views[i]->wasDestroyed())
					{
						window->views[i].reset(window->views.back().release());
						window->views.pop_back();
						continue;
					}
					i++;
				}
				if (!window->screenShotFileName.empty())
				{
					log::warning("Cannot take screenshot, window not visible!");
					window->screenShotFileName.clear();
				}
				return;
			}

			if (!window->screenShotFileName.empty())
			{
				bgfx::requestScreenShot(frameBufferHandle, window->screenShotFileName.c_str());
				window->screenShotFileName.clear();
			}

			_renderContext.currentWindow = this;
			for (size_t i = 0; i < window->views.size(); )
			{
				if (bgfx::isValid(frameBufferHandle))
					bgfx::setViewFrameBuffer(_renderContext.currentViewId, frameBufferHandle);
				if (window->views[i]->wasDestroyed())
				{
					window->views[i].reset(window->views.back().release());
					window->views.pop_back();
					continue;
				}
				window->views[i]->render(_renderContext);
				i++;
			}
		}

		void WindowInstance::postRender()
		{
			for (auto&& view : window->views)
			{
				view->postRender();
			}
		}

		const bool WindowInstance::wasDestroyed() const
		{
			return window == nullptr;
		}

		const uint32_t WindowInstance::getID() const
		{
			if (!window)
				return 0;
			return SDL_GetWindowID(sdlWindow);
		}
		const float WindowInstance::getWidth() const
		{
			se_assert(window);
			return (float)window->getWidth();
		}
		const float WindowInstance::getHeight() const
		{
			se_assert(window);
			return (float)window->getHeight();
		}

		void WindowInstance::update()
		{
			se_assert(window);
			if (checkBit(window->updateFlags, WindowUpdateFlag::PositionChanged))
			{
				SDL_SetWindowPosition(sdlWindow, window->getX(), window->getY());
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::SizeChanged))
			{
				SDL_SetWindowSize(sdlWindow, window->getWidth(), window->getHeight());
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::ResizableChanged))
			{
				SDL_SetWindowResizable(sdlWindow, (SDL_bool)window->getResizable());
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::Minimized))
			{
				SDL_MinimizeWindow(sdlWindow);
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::Maximized))
			{
				SDL_MaximizeWindow(sdlWindow);
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::Restored))
			{
				SDL_RestoreWindow(sdlWindow);
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::InputFocused))
			{
				SDL_SetWindowInputFocus(sdlWindow);
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::Raised))
			{
				SDL_RaiseWindow(sdlWindow);
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::NameChanged))
			{
				SDL_SetWindowTitle(sdlWindow, window->getName().c_str());
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::BorderlessChanged))
			{
				SDL_SetWindowBordered(sdlWindow, (SDL_bool)!window->getBorderless());
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::ConfinedInputChanged))
			{
				SDL_SetWindowGrab(sdlWindow, (SDL_bool)!window->getConfinedInput());
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::ShownChanged))
			{
				if (window->isShown())
					SDL_ShowWindow(sdlWindow);
				else
					SDL_HideWindow(sdlWindow);
			}
			if (checkBit(window->updateFlags, WindowUpdateFlag::OpacityChanged))
			{
				SDL_SetWindowOpacity(sdlWindow, window->getOpacity());
			}
			window->updateFlags = 0;
		}
		void WindowInstance::handleWindowEvent(const SDL_WindowEvent& _event)
		{
			se_assert(window);
			switch (_event.event)
			{
				case SDL_WINDOWEVENT_SHOWN:
				{
					window->shown = true;
					break;
				}
				case SDL_WINDOWEVENT_HIDDEN:
				{
					window->shown = false;
					break;
				}
				case SDL_WINDOWEVENT_MOVED:
				{
					window->x = _event.data1;
					window->y = _event.data2;
					break;
				}
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					const uint16_t newWidth = (uint16_t)_event.data1;
					const uint16_t newHeight = (uint16_t)_event.data2;
					const AspectRatio newAspectRatio(newWidth, newHeight);
					if (window->keepAspectRatio && newAspectRatio != window->aspectRatio)
					{
						const float newAspectHeight = newWidth / ((float)window->aspectRatio.numerator / (float)window->aspectRatio.denominator);
						const float newAspectWidth = newHeight * ((float)window->aspectRatio.numerator / (float)window->aspectRatio.denominator);
						const float widthDifference = abs(newAspectWidth - newWidth);
						const float heightDifference = abs(newAspectHeight - newHeight);
						window->setWidth( widthDifference > heightDifference ? newWidth					 : (uint16_t)newAspectWidth);
						window->setHeight(widthDifference > heightDifference ? (uint16_t)newAspectHeight : newHeight);
					}
					else
					{
						window->aspectRatio = newAspectRatio;
						window->width = newWidth;
						window->height = newHeight;

						if (isDefault)
						{
							resetBackBuffer = true;
						}
						else
						{
							if (bgfx::isValid(frameBufferHandle))
								bgfx::destroy(frameBufferHandle);

							SDL_SysWMinfo wmi;
							SDL_VERSION(&wmi.version);
							if (SDL_GetWindowWMInfo(sdlWindow, &wmi) != SDL_TRUE)
							{
								std::string error = "Unable to get SDL window info: ";
								error += SDL_GetError();
								se::log::error(error);
								break;
							}
							frameBufferHandle = bgfx::createFrameBuffer(wmi.info.win.window, window->getWidth(), window->getHeight());
						}
					}
					break;
				}
				case SDL_WINDOWEVENT_MINIMIZED:
				{
					window->minimized = true;
					break;
				}
				case SDL_WINDOWEVENT_MAXIMIZED:
				{
					window->maximized = true;
					break;
				}
				case SDL_WINDOWEVENT_RESTORED:
				{
					window->minimized = false;
					break;
				}
				case SDL_WINDOWEVENT_ENTER:
				{
					window->mouseFocus = true;
					break;
				}
				case SDL_WINDOWEVENT_LEAVE:
				{
					window->mouseFocus = false;
					break;
				}
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					window->keyboardFocus = true;
					break;
				}
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					window->keyboardFocus = false;
					break;
				}
				case SDL_WINDOWEVENT_CLOSE:
				{
					window->quitRequested = true;
					if (isDefault)
					{
						SDL_Event quitEvent;
						quitEvent.type = SDL_QUIT;
						SDL_PushEvent(&quitEvent);
					}
					break;
				}
			}
		}

		const bool WindowInstance::getBackBufferReset()
		{
			if (resetBackBuffer)
			{
				se_assert(isDefault);
				resetBackBuffer = false;
				return true;
			}
			return false;
		}
	}
}