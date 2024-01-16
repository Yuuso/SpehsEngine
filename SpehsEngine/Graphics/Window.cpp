#include "stdafx.h"
#include "SpehsEngine/Graphics/Window.h"

#include "SDL/SDL_video.h"
#include "SpehsEngine/Graphics/Impl/ViewInternal.h"
#include "SpehsEngine/Graphics/View.h"


Window::Window()
	: aspectRatio(width, height)
{}
Window::Window(bool _hide)
	: Window()
{
	if (_hide)
	{
		hide();
	}
}
Window::~Window()
{
	destroyedSignal();
}
void Window::add(View& _view)
{
	auto it = std::find_if(
		views.begin(),
		views.end(),
		[&_view](const std::unique_ptr<ViewInternal>& view)
		{
			return *view.get() == _view;
		});
	if (it != views.end())
	{
		se::log::error("View already found in window!");
		return;
	}
	views.push_back(std::make_unique<ViewInternal>(_view));
}
void Window::remove(View& _view)
{
	auto it = std::find_if(
		views.begin(),
		views.end(),
		[&_view](const std::unique_ptr<ViewInternal>& view)
		{
			return *view.get() == _view;
		});
	if (it == views.end())
	{
		se::log::error("View not found!");
		return;
	}
	views.erase(it);
}
void Window::takeScreenShot(const std::string& _fileName)
{
	se_assert(!_fileName.empty());
	screenShotFileName = _fileName;
}

const std::string& Window::getName() const
{
	return name;
}
int Window::getX() const
{
	return x;
}
int Window::getY() const
{
	return y;
}
uint16_t Window::getWidth() const
{
	return width;
}
uint16_t Window::getHeight() const
{
	return height;
}
AspectRatio Window::getAspectRatio() const
{
	return aspectRatio;
}
bool Window::getResizable() const
{
	return checkBit(flags, WindowFlags::Resizable);
}
bool Window::getBorderless() const
{
	return checkBit(flags, WindowFlags::Borderless);
}
float Window::getOpacity() const
{
	return opacity;
}
bool Window::isShown() const
{
	return checkBit(flags, WindowFlags::Shown);
}
bool Window::isMinimized() const
{
	return checkBit(flags, WindowFlags::Minimized);
}
bool Window::isMaximized() const
{
	return checkBit(flags, WindowFlags::Maximized);
}
bool Window::isQuitRequested() const
{
	return checkBit(flags, WindowFlags::QuitRequested);
}
bool Window::hasMouseFocus() const
{
	return checkBit(flags, WindowFlags::MouseFocus);
}
bool Window::hasKeyboardFocus() const
{
	return checkBit(flags, WindowFlags::KeyboardFocus);
}
bool Window::getConfinedInput() const
{
	return checkBit(flags, WindowFlags::ConfinedInput);
}
int Window::getDisplayIndex() const
{
	return displayIndex;
}

void Window::setName(const std::string_view _name)
{
	name = _name;
	enableBit(updateFlags, WindowUpdateFlag::NameChanged);
}
void Window::setX(const int _x)
{
	x = _x;
	enableBit(updateFlags, WindowUpdateFlag::PositionChanged);
}
void Window::setY(const int _y)
{
	y = _y;
	enableBit(updateFlags, WindowUpdateFlag::PositionChanged);
}
void Window::setCenteredX()
{
	x = SDL_WINDOWPOS_CENTERED;
	enableBit(updateFlags, WindowUpdateFlag::PositionChanged);
}
void Window::setCenteredY()
{
	y = SDL_WINDOWPOS_CENTERED;
	enableBit(updateFlags, WindowUpdateFlag::PositionChanged);
}
void Window::setWidth(const uint16_t _width)
{
	width = _width;
	aspectRatio = AspectRatio(width, height);
	enableBit(updateFlags, WindowUpdateFlag::SizeChanged);
}
void Window::setHeight(const uint16_t _height)
{
	height = _height;
	aspectRatio = AspectRatio(width, height);
	enableBit(updateFlags, WindowUpdateFlag::SizeChanged);
}
void Window::forceKeepAspectRatio(bool _value)
{
	setBit(flags, WindowFlags::KeepAspectRatio, _value);
}
void Window::setResizable(const bool _value)
{
	setBit(flags, WindowFlags::Resizable, _value);
	enableBit(updateFlags, WindowUpdateFlag::ResizableChanged);
}
void Window::setBorderless(const bool _value)
{
	setBit(flags, WindowFlags::Borderless, _value);
	enableBit(updateFlags, WindowUpdateFlag::BorderlessChanged);
}
void Window::setOpacity(const float _value)
{
	opacity = _value;
	enableBit(updateFlags, WindowUpdateFlag::OpacityChanged);
}
void Window::show()
{
	if (!isShown())
	{
		setBit(flags, WindowFlags::Shown, true);
		enableBit(updateFlags, WindowUpdateFlag::ShownChanged);
	}
}
void Window::hide()
{
	if (isShown())
	{
		setBit(flags, WindowFlags::Shown, false);
		enableBit(updateFlags, WindowUpdateFlag::ShownChanged);
	}
}
void Window::minimize()
{
	if (!isMinimized())
	{
		setBit(flags, WindowFlags::Minimized, true);
		enableBit(updateFlags, WindowUpdateFlag::Minimized);
	}
}
void Window::restore()
{
	if (isMinimized())
	{
		setBit(flags, WindowFlags::Minimized, false);
		enableBit(updateFlags, WindowUpdateFlag::Restored);
	}
}
void Window::maximize()
{
	if (!isMaximized())
	{
		setBit(flags, WindowFlags::Maximized, true);
		enableBit(updateFlags, WindowUpdateFlag::Maximized);
	}
}
void Window::unmaximize()
{
	if (isMaximized())
	{
		setBit(flags, WindowFlags::Maximized, false);
		enableBit(updateFlags, WindowUpdateFlag::Maximized);
	}
}
void Window::ignoreQuitRequest()
{
	disableBit(flags, WindowFlags::QuitRequested);
}
void Window::focus()
{
	enableBit(updateFlags, WindowUpdateFlag::InputFocused);
}
void Window::raise()
{
	enableBit(updateFlags, WindowUpdateFlag::Raised);
}
void Window::setConfinedInput(const bool _value)
{
	setBit(flags, WindowFlags::ConfinedInput, _value);
	enableBit(updateFlags, WindowUpdateFlag::ConfinedInputChanged);
}
SDL_Window* Window::getSDLWindow()
{
	return sdlWindow;
}
