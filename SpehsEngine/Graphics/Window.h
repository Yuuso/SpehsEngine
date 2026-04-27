#pragma once



namespace se
{
	namespace graphics
	{
		class View;
		class ViewInternal;

		class Window
		{
		public:

			typedef glm::vec<2, uint16_t> u16vec2;

									Window();
									Window(bool _hide);
									~Window();

									Window(const Window& _other) = delete;
			Window&					operator=(const Window& _other) = delete;

									Window(Window&& _other) = delete;
			Window&					operator=(Window&& _other) = delete;


			void					add(View& _view);
			void					remove(View& _view);

			void					requestScreenShot(const std::string& _fileName);

			const std::string&		getName() const;
			const int				getX() const;
			const int				getY() const;
			const uint16_t			getWidth() const;
			const uint16_t			getHeight() const;
			const u16vec2			getSize() const;
			const u16vec2			getMinSize() const;
			const u16vec2			getMaxSize() const;
			const AspectRatio		getAspectRatio() const;
			const bool				getResizable() const;
			const bool				getFullscreen() const;
			const bool				getBorderless() const;
			const float				getOpacity() const;
			const bool				isShown() const;
			const bool				getMinimized() const;
			const bool				getMaximized() const;
			const bool				isQuitRequested() const;
			const bool				getMouseFocus() const;
			const bool				getKeyboardFocus() const;
			const bool				getConfinedInput() const;
			const int				getDisplayIndex() const;

			void					setName(const std::string_view _name);
			void					setX(const int _x);
			void					setY(const int _y);
			void					setCenteredX();
			void					setCenteredY();
			void					setWidth(const uint16_t _width);
			void					setHeight(const uint16_t _height);
			void					setSize(const u16vec2& _size);
			void					setMinSize(const u16vec2& _minSize);
			void					setMaxSize(const u16vec2& _maxSize);
			void					forceKeepAspectRatio(const bool _value);
			void					setResizable(const bool _value);
			void					setFullscreen(const bool _value);
			void					setBorderless(const bool _value);
			void					setOpacity(const float _value);
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
			u16vec2					size				= u16vec2(800, 600);
			u16vec2					minSize				= u16vec2(0, 0);
			u16vec2					maxSize				= u16vec2(65535u, 65535u);
			bool					keepAspectRatio		= false;
			AspectRatio				aspectRatio;
			bool					resizable			= true;
			bool					fullscreen			= false;
			bool					borderless			= false;
			float					opacity				= 1.0f;
			int						displayIndex		= -1;

			bool					shown				= true;
			bool					minimized			= false;
			bool					maximized			= false;
			bool					quitRequested		= false;

			bool					mouseFocus			= false;
			bool					keyboardFocus		= false;
			bool					confinedInput		= false;

			std::string				screenShotFileName;

			friend class WindowInternal;
			Signal<void(void)> destroyedSignal;
			std::vector<std::unique_ptr<ViewInternal>> views;
			WindowUpdateFlagsType updateFlags = 0;
		};
	}
}
