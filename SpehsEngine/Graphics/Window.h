#pragma once

struct SDL_Window;

namespace se::gfx
{
	class View;
	namespace impl
	{
		class ViewInternal;
		class WindowInternal;
	}
}

namespace se::gfx
{
	class Window
	{
	public:

		SE_NO_COPY_OR_MOVE(Window);
							Window();
							Window(bool _hide);
							~Window();


		void				add(View& _view);
		void				remove(View& _view);
		void				takeScreenShot(const std::string& _fileName);

		const std::string&	getName() const;
		int					getX() const;
		int					getY() const;
		uint16_t			getWidth() const;
		uint16_t			getHeight() const;
		AspectRatio			getAspectRatio() const;
		bool				getResizable() const;
		bool				getBorderless() const;
		float				getOpacity() const;
		bool				isShown() const;
		bool				isMinimized() const;
		bool				isMaximized() const;
		bool				isQuitRequested() const;
		bool				hasMouseFocus() const;
		bool				hasKeyboardFocus() const;
		bool				getConfinedInput() const;
		int					getDisplayIndex() const;

		void				setName(std::string_view _name);
		void				setX(int _x);
		void				setY(int _y);
		void				setCenteredX();
		void				setCenteredY();
		void				setWidth(uint16_t _width);
		void				setHeight(uint16_t _height);
		void				forceKeepAspectRatio(bool _value);
		void				setResizable(bool _value);
		void				setBorderless(bool _value);
		void				setOpacity(float _value);
		void				show();
		void				hide();
		void				minimize();
		void				restore();
		void				maximize();
		void				unmaximize();
		void				ignoreQuitRequest();
		void				focus();
		void				raise();
		void				setConfinedInput(bool _value);

		SDL_Window *		getSDLWindow();

	private:

		enum WindowFlags : int
		{
			None				= 0,
			KeepAspectRatio		= (1 << 0),
			Resizable			= (1 << 1),
			Borderless			= (1 << 2),
			Shown				= (1 << 3),
			Minimized			= (1 << 4),
			Maximized			= (1 << 5),
			QuitRequested		= (1 << 6),
			MouseFocus			= (1 << 7),
			KeyboardFocus		= (1 << 8),
			ConfinedInput		= (1 << 9),
		};

		std::string				name				= "SpehsEngine";
		std::string				screenShotFileName;
		int						x					= -1;
		int						y					= -1;
		uint16_t				width				= 800;
		uint16_t				height				= 600;
		AspectRatio				aspectRatio;
		float					opacity				= 1.0f;
		int						displayIndex		= -1;
		WindowFlags				flags				= WindowFlags{Resizable | Shown};

		friend class impl::WindowInternal;
		Signal<void(void)> destroyedSignal;
		std::vector<std::unique_ptr<impl::ViewInternal>> views;
		WindowUpdateFlagsType updateFlags = 0;
		SDL_Window* sdlWindow = nullptr;
	};
}
