#pragma once

#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"
#include "glm/vec2.hpp"
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
			Renderer(Window& _window, const RendererFlagsType _rendererFlags);
			~Renderer();

			Renderer(const Renderer& _other) = delete;
			Renderer& operator=(const Renderer& _other) = delete;

			Renderer(Renderer&& _other) = delete;
			Renderer& operator=(Renderer&& _other) = delete;


			void render();

			void add(Window& _window);
			void remove(Window& _window);

			const RendererFlagsType getRenderOptions() const;
			const bool checkRenderOption(const RendererFlag _rendererFlag) const;
			void setRenderOptions(const RendererFlagsType _rendererFlags);
			void enableRenderOption(const RendererFlag _rendererFlag);
			void disableRenderOption(const RendererFlag _rendererFlag);

			const glm::ivec2 getDisplaySize() const;
			const int getDisplayRefreshRate() const;


			static void debugTextPrintf(const uint16_t _column, const uint16_t _line, const char* _format, ...);

		private:

			void inputUpdate();

			std::vector<std::unique_ptr<WindowInstance>> windows;
			WindowInstance* defaultWindow = nullptr;

			RendererFlagsType rendererFlags;
			bool rendererFlagsChanged = false;
		};
	}
}
