#pragma once

#include "glm/vec2.hpp"
#include "SpehsEngine/Graphics/Internal/DefaultUniforms.h"
#include "SpehsEngine/Graphics/Internal/WindowInstance.h"
#include "SpehsEngine/Graphics/Window.h"
#include <memory>
#include <vector>


namespace se
{
	namespace graphics
	{
		class Renderer
		{
		public:

			Renderer(Window& _window);
			Renderer(Window& _window, const RendererFlagsType _rendererFlags, const RendererBackend _rendererBackend = RendererBackend::Auto);
			~Renderer();

			Renderer(const Renderer& _other) = delete;
			Renderer& operator=(const Renderer& _other) = delete;

			Renderer(Renderer&& _other) = delete;
			Renderer& operator=(Renderer&& _other) = delete;


			void render();

			void add(Window& _window);
			void remove(Window& _window);

			const RendererFlagsType getRendererFlags() const;
			const bool checkRendererFlag(const RendererFlag _rendererFlag) const;
			void setRendererFlags(const RendererFlagsType _rendererFlags);
			void enableRendererFlag(const RendererFlag _rendererFlag);
			void disableRendererFlag(const RendererFlag _rendererFlag);

			static const RendererBackend getRendererBackend();

			const glm::ivec2 getDisplaySize() const;
			const int getDisplayRefreshRate() const;


			static void debugTextPrintf(const uint16_t _column, const uint16_t _line, const char* _format, ...);

			void reloadDefaultUniforms(); // TODO: ?!!?!?!?

		private:

			void inputUpdate();

			inline static bool initialized = false;

			std::vector<std::unique_ptr<WindowInstance>> windows;
			WindowInstance* defaultWindow = nullptr;

			RendererFlagsType rendererFlags;
			bool rendererFlagsChanged = false;

			std::unique_ptr<DefaultUniforms> defaultUniforms;
		};
	}
}
