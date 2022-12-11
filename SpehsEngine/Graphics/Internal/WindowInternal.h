#pragma once

struct SDL_Window;
struct SDL_WindowEvent;


namespace se
{
	namespace graphics
	{
		struct RenderContext;
		class Window;

		class WindowInternal
		{
		public:

			WindowInternal(Window& _window, const bool _isDefault);
			~WindowInternal();

			WindowInternal(const WindowInternal& _other) = delete;
			WindowInternal& operator=(const WindowInternal& _other) = delete;

			WindowInternal(WindowInternal&& _other) = delete;
			WindowInternal& operator=(WindowInternal&& _other) = delete;

			bool operator==(const Window& _other) const;


			void render(RenderContext& _renderContext);
			void preRender();
			void postRender();

			const bool wasDestroyed() const;

			const uint32_t getID() const;
			const float getWidth() const;
			const float getHeight() const;

			void update();
			void handleWindowEvent(const SDL_WindowEvent& _event);

			const bool isDefault;
			const bool getBackBufferReset();

			SDL_Window* getSDLWindow();
			void* getNativeWindowHandle();

		private:

			void windowDestroyed();
			bool renderState() const;

			boost::signals2::scoped_connection windowDestroyedConnection;

			Window* window;

			SDL_Window* sdlWindow = nullptr;
			bgfx::FrameBufferHandle frameBufferHandle = BGFX_INVALID_HANDLE;
			bool resetBackBuffer = false;
			bool wasAdded = true;
		};
	}
}
