#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/WindowInternal.h"

#include "SpehsEngine/Graphics/Impl/ViewInternal.h"
#include "SpehsEngine/Graphics/Window.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

#include "bgfx/platform.h"
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#include "SDL/SDL_events.h"
#include "SDL/SDL_video.h"

#pragma comment(lib, "psapi.lib")


WindowInternal::WindowInternal(Window& _window, const bool _isDefault)
	: window(&_window)
	, isDefault(_isDefault)
{
	windowDestroyedConnection = window->destroyedSignal.connect(std::bind(&WindowInternal::windowDestroyed, this));

	uint32_t windowFlags = 0;
	if (!window->isShown())
		enableBit(windowFlags, SDL_WINDOW_HIDDEN);
	if (window->getBorderless())
		enableBit(windowFlags, SDL_WINDOW_BORDERLESS);
	if (window->getResizable())
		enableBit(windowFlags, SDL_WINDOW_RESIZABLE);
	if (window->isMinimized())
		enableBit(windowFlags, SDL_WINDOW_MINIMIZED);
	if (window->isMaximized())
		enableBit(windowFlags, SDL_WINDOW_MAXIMIZED);
	if (window->hasKeyboardFocus())
		enableBit(windowFlags, SDL_WINDOW_INPUT_FOCUS);
	if (window->hasMouseFocus())
		enableBit(windowFlags, SDL_WINDOW_MOUSE_FOCUS);
	if (window->getConfinedInput())
		enableBit(windowFlags, SDL_WINDOW_INPUT_GRABBED);
	//SDL_WINDOW_ALLOW_HIGHDPI
	//SDL_WINDOW_OPENGL
	//SDL_WINDOW_VULKAN

	const int x = window->getX() != -1 ? window->getX() : SDL_WINDOWPOS_UNDEFINED;
	const int y = window->getY() != -1 ? window->getY() : SDL_WINDOWPOS_UNDEFINED;

	sdlWindow = SDL_CreateWindow(
		window->getName().c_str(),
		x,
		y,
		window->getWidth(),
		window->getHeight(),
		windowFlags);

	if (!sdlWindow)
	{
		se::log::fatal("Failed to create a window!");
		return;
	}

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
	window->displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
	window->sdlWindow = sdlWindow;

	if (!isDefault)
	{
		frameBufferHandle = bgfx::createFrameBuffer(getNativeWindowHandle(), window->getWidth(), window->getHeight());
	}
}
WindowInternal::~WindowInternal()
{
	if (bgfx::isValid(frameBufferHandle))
	{
		bgfx::destroy(frameBufferHandle);
		frameBufferHandle = BGFX_INVALID_HANDLE;
	}
	SDL_DestroyWindow(sdlWindow);
}

void WindowInternal::windowDestroyed()
{
	window = nullptr;
}

bool WindowInternal::operator==(const Window& _other) const
{
	return window == &_other;
}

void WindowInternal::render(RenderContext& _renderContext)
{
	if (!window->screenShotFileName.empty())
	{
		if (renderState())
		{
			bgfx::requestScreenShot(frameBufferHandle, window->screenShotFileName.c_str());
		}
		else
		{
			log::warning("Cannot take screenshot, window not visible!");
		}
		window->screenShotFileName.clear();
	}

	if (!renderState())
		return;

	_renderContext.currentWindow = this;
	for (size_t i = 0; i < window->views.size(); )
	{
		bgfx::setViewFrameBuffer(_renderContext.currentViewId, frameBufferHandle);
		se_assert(!window->views[i]->wasDestroyed());
		window->views[i]->render(_renderContext);
		i++;
	}
}

void WindowInternal::preRender()
{
	se_assert(window);
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
	for (auto&& view : window->views)
	{
		view->preRender(this, renderState(), wasAdded);
	}
}
void WindowInternal::postRender()
{
	for (auto&& view : window->views)
	{
		view->postRender(renderState());
	}
	wasAdded = false;
}

bool WindowInternal::renderState() const
{
	return !window->isMinimized() && window->isShown();
}

const bool WindowInternal::wasDestroyed() const
{
	return window == nullptr;
}

const uint32_t WindowInternal::getID() const
{
	if (!window)
		return 0;
	return SDL_GetWindowID(sdlWindow);
}
const float WindowInternal::getWidth() const
{
	se_assert(window);
	return (float)window->getWidth();
}
const float WindowInternal::getHeight() const
{
	se_assert(window);
	return (float)window->getHeight();
}

void WindowInternal::update()
{
	se_assert(window);
	if (checkBit(window->updateFlags, WindowUpdateFlag::SizeChanged))
	{
		SDL_SetWindowSize(sdlWindow, window->getWidth(), window->getHeight());
	}
	if (checkBit(window->updateFlags, WindowUpdateFlag::PositionChanged))
	{
		SDL_SetWindowPosition(sdlWindow, window->getX(), window->getY());
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
	if (checkBit(window->updateFlags, WindowUpdateFlag::OpacityChanged))
	{
		SDL_SetWindowOpacity(sdlWindow, window->getOpacity());
	}
	if (checkBit(window->updateFlags, WindowUpdateFlag::ShownChanged))
	{
		if (window->isShown())
			SDL_ShowWindow(sdlWindow);
		else
			SDL_HideWindow(sdlWindow);
	}
	window->updateFlags = 0;
}
void WindowInternal::handleWindowEvent(const SDL_WindowEvent& _event)
{
	se_assert(window);
	switch (_event.event)
	{
		case SDL_WINDOWEVENT_SHOWN:
		{
			if (!checkBit(window->updateFlags, WindowUpdateFlag::ShownChanged))
			{
				enableBit(window->flags, Window::WindowFlags::Shown);
			}
			break;
		}
		case SDL_WINDOWEVENT_HIDDEN:
		{
			if (!checkBit(window->updateFlags, WindowUpdateFlag::ShownChanged))
			{
				disableBit(window->flags, Window::WindowFlags::Shown);
			}
			break;
		}
		case SDL_WINDOWEVENT_MOVED:
		{
			if (!checkBit(window->updateFlags, WindowUpdateFlag::PositionChanged))
			{
				window->x = _event.data1;
				window->y = _event.data2;
			}
			window->displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
			break;
		}
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		{
			const uint16_t newWidth = (uint16_t)_event.data1;
			const uint16_t newHeight = (uint16_t)_event.data2;
			const AspectRatio newAspectRatio(newWidth, newHeight);
			if (checkBit(window->flags, Window::WindowFlags::KeepAspectRatio) && newAspectRatio != window->aspectRatio)
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

				if (!checkBit(window->updateFlags, WindowUpdateFlag::SizeChanged))
				{
					window->width = newWidth;
					window->height = newHeight;
				}

				if (isDefault)
				{
					resetBackBuffer = true;
				}
				else
				{
					if (bgfx::isValid(frameBufferHandle))
					{
						bgfx::destroy(frameBufferHandle);
					}

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
			if (!checkBit(window->updateFlags, WindowUpdateFlag::Minimized) &&
					checkBit(window->updateFlags, WindowUpdateFlag::Restored))
			{
				enableBit(window->flags, Window::WindowFlags::Minimized);
			}
			break;
		}
		case SDL_WINDOWEVENT_MAXIMIZED:
		{
			if (!checkBit(window->updateFlags, WindowUpdateFlag::Maximized))
			{
				disableBit(window->flags, Window::WindowFlags::Minimized);
			}
			break;
		}
		case SDL_WINDOWEVENT_RESTORED:
		{
			if (!checkBit(window->updateFlags, WindowUpdateFlag::Minimized) &&
				checkBit(window->updateFlags, WindowUpdateFlag::Restored))
			{
				disableBit(window->flags, Window::WindowFlags::Minimized);
			}
			break;
		}
		case SDL_WINDOWEVENT_ENTER:
		{
			enableBit(window->flags, Window::WindowFlags::MouseFocus);
			break;
		}
		case SDL_WINDOWEVENT_LEAVE:
		{
			disableBit(window->flags, Window::WindowFlags::MouseFocus);
			break;
		}
		case SDL_WINDOWEVENT_FOCUS_GAINED:
		{
			enableBit(window->flags, Window::WindowFlags::KeyboardFocus);
			break;
		}
		case SDL_WINDOWEVENT_FOCUS_LOST:
		{
			disableBit(window->flags, Window::WindowFlags::KeyboardFocus);
			break;
		}
		case SDL_WINDOWEVENT_CLOSE:
		{
			enableBit(window->flags, Window::WindowFlags::QuitRequested);
			break;
		}
	}
}

const bool WindowInternal::getBackBufferReset()
{
	if (resetBackBuffer)
	{
		se_assert(isDefault);
		resetBackBuffer = false;
		return true;
	}
	return false;
}

SDL_Window* WindowInternal::getSDLWindow()
{
	return sdlWindow;
}
void* WindowInternal::getNativeWindowHandle()
{
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (SDL_GetWindowWMInfo(sdlWindow, &wmi) != SDL_TRUE)
	{
		std::string error = "Unable to get SDL window info: ";
		error += SDL_GetError();
		se::log::error(error);
		return nullptr;
	}
	return wmi.info.win.window;
}