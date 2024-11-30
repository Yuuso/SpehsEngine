#pragma once

struct SDL_Window;
struct SDL_WindowEvent;

namespace se::gfx
{
	class Window;
	namespace impl
	{
		struct RenderContext;
	}
}

namespace se::gfx::impl
{
	class WindowInternal
	{
	public:

		SE_NO_COPY_OR_MOVE(WindowInternal);
		WindowInternal(Window& _window, const bool _isDefault);
		~WindowInternal();

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

		ScopedConnection windowDestroyedConnection;

		Window* window = nullptr;

		SDL_Window* sdlWindow = nullptr;
		bgfx::FrameBufferHandle frameBufferHandle = BGFX_INVALID_HANDLE;
		bool resetBackBuffer = false;
		bool wasAdded = true;
	};
}
