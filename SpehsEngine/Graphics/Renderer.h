#pragma once

namespace se::gfx
{
	class Texture;
	class Window;
	namespace impl
	{
		class DefaultUniforms;
		class RendererCallbackHandler;
		class WindowInternal;
	}
}

namespace se::gfx
{
	class Renderer
	{
	public:

		static RendererBackend getBackend();

		SE_NO_COPY_OR_MOVE(Renderer);
		Renderer(Window& _window);
		Renderer(Window& _window, RendererFlagsType _rendererFlags, RendererBackend _rendererBackend = RendererBackend::Auto);
		~Renderer();

		void render();
		void add(Window& _window);
		void remove(Window& _window);

		RendererFlagsType getRendererFlags() const;
		bool checkRendererFlag(RendererFlag _rendererFlag) const;
		void setRendererFlags(RendererFlagsType _rendererFlags);
		void enableRendererFlag(RendererFlag _rendererFlag);
		void disableRendererFlag(RendererFlag _rendererFlag);

		void setFallbackTexture(std::shared_ptr<Texture> _texture);

	private:

		void inputUpdate();

		inline static bool initialized = false;

		std::vector<std::unique_ptr<impl::WindowInternal>> windows;
		impl::WindowInternal* defaultWindow = nullptr;
		std::shared_ptr<Texture> fallbackTexture;
		RendererFlagsType rendererFlags;
		bool rendererFlagsChanged = false;
		std::unique_ptr<impl::DefaultUniforms> defaultUniforms;
		impl::RendererCallbackHandler* callbackHandler = nullptr;
	};
}
