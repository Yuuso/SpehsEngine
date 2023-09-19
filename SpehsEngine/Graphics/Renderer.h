#pragma once

struct SDL_Window;

namespace se::gfx::impl
{
	class DefaultUniforms;
	class RendererCallbackHandler;
	class WindowInternal;
}

namespace se::gfx
{
	class Window;

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

		SDL_Window* getDefaultSDLWindow(); // Ugly, plz cleanup


		static const RendererBackend getRendererBackend();
		static const glm::ivec2 getDisplaySize();
		static const glm::vec2 getDisplayDPI();
		static const std::vector<DisplayMode> getDisplayModes(const int _displayIndex);
		static const int getDisplayRefreshRate();

		static void debugTextPrintf(const uint16_t _column, const uint16_t _line, const char* _format, ...);

	private:

		void inputUpdate();

		inline static bool initialized = false;

		std::vector<std::unique_ptr<impl::WindowInternal>> windows;
		impl::WindowInternal* defaultWindow = nullptr;

		RendererFlagsType rendererFlags;
		bool rendererFlagsChanged = false;

		std::unique_ptr<impl::DefaultUniforms> defaultUniforms;

		impl::RendererCallbackHandler* callbackHandler = nullptr;
	};
}
