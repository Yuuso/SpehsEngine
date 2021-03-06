#pragma once

#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "bgfx/bgfx.h" // TODO: No bgfx includes in headers!


struct SDL_Window;
struct SDL_WindowEvent;


namespace se
{
	namespace graphics
	{
		class WindowInstance
		{
		public:

			WindowInstance(Window& _window, const bool _isDefault);
			~WindowInstance();

			WindowInstance(const WindowInstance& _other) = delete;
			WindowInstance& operator=(const WindowInstance& _other) = delete;

			WindowInstance(WindowInstance&& _other) = delete;
			WindowInstance& operator=(WindowInstance&& _other) = delete;

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
