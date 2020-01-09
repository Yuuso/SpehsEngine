#pragma once

#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/Graphics/Internal/RenderContext.h"
#include "bgfx/bgfx.h" // TODO: No bgfx includes in headers!


struct SDL_Window;


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


			bool render(RenderContext& _renderContext);

			float getWidth() const;
			float getHeight() const;

			const bool isDefault;

		private:

			void windowDestroyed();

			boost::signals2::scoped_connection windowDestroyedConnection;

			Window* window;

			SDL_Window* sdlWindow = nullptr;
			bgfx::FrameBufferHandle frameBufferHandle = BGFX_INVALID_HANDLE;
		};
	}
}
