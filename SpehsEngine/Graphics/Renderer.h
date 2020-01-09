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

			Renderer(Window& _window);
			~Renderer();

			Renderer(const Renderer& _other) = delete;
			Renderer& operator=(const Renderer& _other) = delete;

			Renderer(Renderer&& _other) = delete;
			Renderer& operator=(Renderer&& _other) = delete;


			void render();

			void add(Window& _window);
			void remove(Window& _window);

			static void debugTextPrintf(const uint16_t _column, const uint16_t _line, const char* _format, ...);

		private:

			std::vector<std::unique_ptr<WindowInstance>> windows;
		};
	}
}
