#pragma once

#include "SpehsEngine/Graphics/Window.h"
#include "bgfx/bgfx.h" // TODO: No bgfx includes in headers!


struct SDL_Window;


namespace se
{
	namespace graphics
	{
		class WindowInstance
		{
		public:

			WindowInstance(const Window& _window, const bool _isDefault);
			~WindowInstance();

			WindowInstance(const WindowInstance& _other) = delete;
			WindowInstance& operator=(const WindowInstance& _other) = delete;

			WindowInstance(WindowInstance&& _other) = delete;
			WindowInstance& operator=(WindowInstance&& _other) = delete;

			bool operator==(const Window& _other);


			void render();

			const bool isDefault;

		private:

			const Window& window;
			SDL_Window* sdlWindow = nullptr;
			bgfx::FrameBufferHandle frameBufferHandle;
		};
	}
}
