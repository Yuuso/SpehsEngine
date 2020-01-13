#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Internal/ViewInstance.h"
#include <vector>
#include <memory>



namespace se
{
	namespace graphics
	{
		class Window
		{
		public:

									Window();
									~Window();

									Window(const Window& _other) = delete;
			Window&					operator=(const Window& _other) = delete;

									Window(Window&& _other) = delete;
			Window&					operator=(Window&& _other) = delete;


			void					add(View& _view);
			void					remove(View& _view);

			const std::string&		getName() const;
			const int				getX() const;
			const int				getY() const;
			const uint16_t			getWidth() const;
			const uint16_t			getHeight() const;
			const AspectRatio		getAspectRatio() const;
			const bool				getResizable() const;
			const bool				getBorderless() const;
			const bool				isShown() const;
			const bool				getMinimized() const;
			const bool				getMaximized() const;
			const bool				isQuitRequested() const;
			const bool				getMouseFocus() const;
			const bool				getKeyboardFocus() const;
			const bool				getConfinedInput() const;

			void					setName(const std::string_view _name);
			void					setX(const int _x);
			void					setY(const int _y);
			void					setWidth(const uint16_t _width);
			void					setHeight(const uint16_t _height);
			void					forceKeepAspectRatio(const bool _value);
			void					setResizable(const bool _value);
			void					setBorderless(const bool _value);
			void					show();
			void					hide();
			void					minimize();
			void					restore();
			void					maximize();
			void					ignoreQuitRequest();
			void					focus();
			void					raise();
			void					setConfinedInput(const bool _value);

		private:

			std::string				name				= "SpehsEngine";

			int						x					= -1;
			int						y					= -1;
			uint16_t				width				= 800;
			uint16_t				height				= 600;
			bool					keepAspectRatio		= false;
			AspectRatio				aspectRatio;
			bool					resizable			= true;
			bool					borderless			= false;

			bool					shown				= true;
			bool					minimized			= false;
			bool					maximized			= false;
			bool					quitRequested		= false;

			bool					mouseFocus			= false;
			bool					keyboardFocus		= false;
			bool					confinedInput		= false;


			friend class WindowInstance;
			boost::signals2::signal<void(void)> destroyedSignal;
			std::vector<std::unique_ptr<ViewInstance>> views;
			WindowUpdateFlagsType updateFlags = 0;
		};
	}
}
