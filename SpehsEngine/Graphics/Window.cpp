#include "stdafx.h"
#include "SpehsEngine/Graphics/Window.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"


namespace se
{
	namespace graphics
	{
		Window::Window()
			: aspectRatio(width, height)
		{
		}
		Window::~Window()
		{
			destroyedSignal();
		}

		void Window::add(View& _view)
		{
			auto it = std::find_if(views.begin(),
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
			auto it = std::find_if(views.begin(),
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


		void Window::requestScreenShot(const std::string& _fileName)
		{
			se_assert(!_fileName.empty());
			screenShotFileName = _fileName;
		}


		const std::string& Window::getName() const
		{
			return name;
		}
		const int Window::getX() const
		{
			return x;
		}
		const int Window::getY() const
		{
			return y;
		}
		const uint16_t Window::getWidth() const
		{
			return width;
		}
		const uint16_t Window::getHeight() const
		{
			return height;
		}
		const AspectRatio Window::getAspectRatio() const
		{
			return aspectRatio;
		}
		const bool Window::getResizable() const
		{
			return resizable;
		}
		const bool Window::getBorderless() const
		{
			return borderless;
		}
		const float Window::getOpacity() const
		{
			return opacity;
		}
		const bool Window::isShown() const
		{
			return shown;
		}
		const bool Window::getMinimized() const
		{
			return minimized;
		}
		const bool Window::getMaximized() const
		{
			return maximized;
		}
		const bool Window::isQuitRequested() const
		{
			return quitRequested;
		}
		const bool Window::getMouseFocus() const
		{
			return mouseFocus;
		}
		const bool Window::getKeyboardFocus() const
		{
			return keyboardFocus;
		}
		const bool Window::getConfinedInput() const
		{
			return confinedInput;
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
		void Window::forceKeepAspectRatio(const bool _value)
		{
			keepAspectRatio = _value;
		}
		void Window::setResizable(const bool _value)
		{
			resizable = _value;
			enableBit(updateFlags, WindowUpdateFlag::ResizableChanged);
		}
		void Window::setBorderless(const bool _value)
		{
			borderless = _value;
			enableBit(updateFlags, WindowUpdateFlag::BorderlessChanged);
		}
		void Window::setOpacity(const float _value)
		{
			opacity = _value;
			enableBit(updateFlags, WindowUpdateFlag::OpacityChanged);
		}
		void Window::show()
		{
			if (!shown)
			{
				shown = true;
				enableBit(updateFlags, WindowUpdateFlag::ShownChanged);
			}
		}
		void Window::hide()
		{
			if (shown)
			{
				shown = false;
				enableBit(updateFlags, WindowUpdateFlag::ShownChanged);
			}
		}
		void Window::minimize()
		{
			if (!minimized)
			{
				minimized = true;
				enableBit(updateFlags, WindowUpdateFlag::Minimized);
			}
		}
		void Window::restore()
		{
			if (minimized)
			{
				minimized = false;
				enableBit(updateFlags, WindowUpdateFlag::Restored);
			}
		}
		void Window::maximize()
		{
			if (!maximized)
			{
				maximized = true;
				enableBit(updateFlags, WindowUpdateFlag::Maximized);
			}
		}
		void Window::ignoreQuitRequest()
		{
			quitRequested = false;
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
			confinedInput = _value;
			enableBit(updateFlags, WindowUpdateFlag::ConfinedInputChanged);
		}
	}
}
