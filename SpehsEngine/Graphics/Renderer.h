#pragma once

#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"
#include <vector>
#include <memory>


namespace se
{
	namespace graphics
	{
		class Renderer
		{
		public:

			Renderer(const Window& _window);
			~Renderer();

			Renderer(const Renderer& _other) = delete;
			Renderer& operator=(const Renderer& _other) = delete;

			Renderer(Renderer&& _other) = delete;
			Renderer& operator=(Renderer&& _other) = delete;


			void render();

			void add(const Window& _window);
			void remove(const Window& _window);

			void debugTextPrintf(const uint16_t _x, const uint16_t _y, const char* _format, ...);

		private:

			std::vector<std::unique_ptr<WindowInstance>> windows;
		};
	}
}
