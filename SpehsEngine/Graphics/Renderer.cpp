#include "stdafx.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include "bgfx/platform.h"
#include "SDL/SDL.h"
#include "SDL/SDL_events.h"
#include "SpehsEngine/Core/HexColor.h"
#include "SpehsEngine/Graphics/GraphicsUtilities.h"
#include "SpehsEngine/Graphics/Impl/DefaultUniforms.h"
#include "SpehsEngine/Graphics/Impl/WindowInternal.h"
#include "SpehsEngine/Graphics/Impl/RendererCallbackHandler.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Window.h"


Renderer::Renderer(Window& _window, RendererFlagsType _rendererFlags, RendererBackend _rendererBackend)
	: rendererFlags(_rendererFlags)
{
	if (initialized)
	{
		log::error("You can only have one Renderer!");
		return;
	}
	initialized = true;

	windows.push_back(std::make_unique<WindowInternal>(_window, true));
	defaultWindow = windows.back().get();

	const bgfx::RendererType::Enum rendererType = getRendererType(_rendererBackend);

	{
		callbackHandler = new RendererCallbackHandler;

		bgfx::Init init;

	#if defined(_WIN32)
		init.platformData.ndt = nullptr;
		init.platformData.nwh = defaultWindow->getNativeWindowHandle();
	#else
		#error Window platform data handling not implemented!
	#endif
		init.platformData.context = nullptr;
		init.platformData.backBuffer = nullptr;
		init.platformData.backBufferDS = nullptr;

		init.type = rendererType;
		init.resolution.width = (uint32_t)_window.getWidth();
		init.resolution.height = (uint32_t)_window.getHeight();
		init.resolution.reset = getResetParameters(rendererFlags);
		init.callback = callbackHandler;
		const bool success = bgfx::init(init);
		if (!success)
			log::fatal("Failed to initialize Renderer!");

		bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);
	}

	log::info("Graphics Renderer initialized:");

	// Renderer capabilities
	{
		const bgfx::Caps* caps = bgfx::getCaps();
		se_assert(caps);
		se_assert(caps->rendererType == rendererType ||
			_rendererBackend == RendererBackend::Auto);

		log::info("  Renderer type: " + toString(getRendererBackend()));

		// Check support for features we might use
		{
			#define CAPS_CHECK(_c, _warn) \
			if (!checkBit(caps->supported, _c)) \
				log::info(std::string("  Renderer: ") + _warn, log::YELLOW);

			CAPS_CHECK(BGFX_CAPS_ALPHA_TO_COVERAGE,			"Alpha to coverage not supported!")
			CAPS_CHECK(BGFX_CAPS_BLEND_INDEPENDENT,			"Blend independent not supported!")
			CAPS_CHECK(BGFX_CAPS_COMPUTE,					"Compute shaders not supported!")
			CAPS_CHECK(BGFX_CAPS_DRAW_INDIRECT,				"Draw indirect not supported!")
			CAPS_CHECK(BGFX_CAPS_INSTANCING,				"Instancing not supported!")
			CAPS_CHECK(BGFX_CAPS_HDR10,						"HDR10 not supported!")
			CAPS_CHECK(BGFX_CAPS_OCCLUSION_QUERY,			"Occlusion query not supported!")
			CAPS_CHECK(BGFX_CAPS_RENDERER_MULTITHREADED,	"Not rendering multithreaded!")
			CAPS_CHECK(BGFX_CAPS_SWAP_CHAIN,				"Multiple windows not supported!")
			CAPS_CHECK(BGFX_CAPS_TEXTURE_BLIT,				"Texture blit not supported!")
			CAPS_CHECK(BGFX_CAPS_TRANSPARENT_BACKBUFFER,	"Transparent back buffer not supported!")
			CAPS_CHECK(BGFX_CAPS_VERTEX_ATTRIB_HALF,		"Half-float vertex attribute not supported!")
			CAPS_CHECK(BGFX_CAPS_VERTEX_ATTRIB_UINT10,		"10_10_10_2 vertex attribute not supported!")
			CAPS_CHECK(BGFX_CAPS_VERTEX_ID,					"Auto instancing not supported!")
		}

		// GPU vendor
		{
			switch (caps->vendorId)
			{
				case BGFX_PCI_ID_NONE:					log::info("  Renderer PCI ID: Unknown"); break;
				case BGFX_PCI_ID_SOFTWARE_RASTERIZER:	log::info("  Renderer PCI ID: Software Rasterizer"); break;
				case BGFX_PCI_ID_AMD:					log::info("  Renderer PCI ID: AMD adapter"); break;
				case BGFX_PCI_ID_APPLE:					log::info("  Renderer PCI ID: Apple adapter"); break;
				case BGFX_PCI_ID_INTEL:					log::info("  Renderer PCI ID: Intel adapter"); break;
				case BGFX_PCI_ID_NVIDIA:				log::info("  Renderer PCI ID: Nvidia adapter"); break;
				case BGFX_PCI_ID_MICROSOFT:				log::info("  Renderer PCI ID: Microsoft adapter"); break;
				case BGFX_PCI_ID_ARM:					log::info("  Renderer PCI ID: ARM adapter"); break;
			}
		}
	}

	{
		TextureInput input(2, 2);
		input.push(Color{ HexColor::Pink });
		input.push(Color{ HexColor::Pink });
		input.push(Color{ HexColor::Pink });
		input.push(Color{ HexColor::Pink });
		TextureModes modes;
		modes.sampleMin = TextureSamplingMode::Point;
		modes.sampleMag = TextureSamplingMode::Point;
		modes.sampleMip = TextureSamplingMode::Point;
		fallbackTexture = std::make_shared<Texture>("fallback_texture", nullptr, input, modes);
	}

	defaultUniforms = std::make_unique<DefaultUniforms>();
}
Renderer::Renderer(Window& _window)
	: Renderer(_window, 0)
{}
Renderer::~Renderer()
{
	se_assert(initialized);
	windows.clear();
	fallbackTexture.reset();
	defaultUniforms.reset();
	bgfx::shutdown();
	while (bgfx::renderFrame() != bgfx::RenderFrame::NoContext) {}
	if (callbackHandler)
		delete callbackHandler;
	initialized = false;
}
RendererBackend Renderer::getBackend()
{
	se_assert(initialized);
	return impl::getRendererBackend();
}
void Renderer::render()
{
	if (!defaultWindow || defaultWindow->wasDestroyed())
	{
		log::error("Render called after default window was destroyed!");
		return;
	}

	inputUpdate();

	if (rendererFlagsChanged || defaultWindow->getBackBufferReset())
	{
		bgfx::reset((uint32_t)defaultWindow->getWidth(),
					(uint32_t)defaultWindow->getHeight(),
					getResetParameters(rendererFlags));
		rendererFlagsChanged = false;
	}

	for (size_t i = 0; i < windows.size(); )
	{
		if (windows[i]->wasDestroyed())
		{
			windows[i].reset(windows.back().release());
			windows.pop_back();
			continue;
		}
		i++;
	}

	for (auto&& window : windows)
	{
		window->update();
		window->preRender();
	}
	RenderContext renderContext;
	renderContext.rendererFlags = rendererFlags;
	renderContext.defaultUniforms = defaultUniforms.get();
	renderContext.fallbackTexture = fallbackTexture.get();
	defaultUniforms->updateEngineUtility();
	for (auto&& window : windows)
	{
		window->render(renderContext);
	}
	bgfx::frame();
	bgfx::dbgTextClear();
	for (auto&& window : windows)
	{
		window->postRender();
	}
}
void Renderer::add(Window& _window)
{
	auto it = std::find_if(windows.begin(),
							windows.end(),
							[&_window](const std::unique_ptr<WindowInternal>& window)
							{
								return *window.get() == _window;
							});
	if (it != windows.end())
	{
		se::log::error("Window already found in renderer!");
		return;
	}
	windows.push_back(std::make_unique<WindowInternal>(_window, false));
}
void Renderer::remove(Window& _window)
{
	auto it = std::find_if(windows.begin(),
							windows.end(),
							[&_window](const std::unique_ptr<WindowInternal>& window)
							{
								return *window.get() == _window;
							});
	if (it == windows.end())
	{
		se::log::error("Window not found!");
		return;
	}
	if (it->get() == defaultWindow)
	{
		defaultWindow = nullptr;
	}
	windows.erase(it);
}
RendererFlagsType Renderer::getRendererFlags() const
{
	return rendererFlags;
}
bool Renderer::checkRendererFlag(RendererFlag _renderFlag) const
{
	return checkBit(rendererFlags, _renderFlag);
}
void Renderer::setRendererFlags(RendererFlagsType _rendererFlags)
{
	rendererFlags = _rendererFlags;
	rendererFlagsChanged = true;
}
void Renderer::enableRendererFlag(RendererFlag _renderFlag)
{
	enableBit(rendererFlags, _renderFlag);
	rendererFlagsChanged = true;
}
void Renderer::disableRendererFlag(RendererFlag _renderFlag)
{
	disableBit(rendererFlags, _renderFlag);
	rendererFlagsChanged = true;
}
void Renderer::setFallbackTexture(std::shared_ptr<Texture> _texture)
{
	fallbackTexture = _texture;
}
void Renderer::inputUpdate()
{
	static const size_t MAX_EVENTS = 256;
	SDL_Event sdlEvents[MAX_EVENTS];

	const int numWindowEvents = SDL_PeepEvents(sdlEvents, (int)MAX_EVENTS, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_SYSWMEVENT);
	if (numWindowEvents > 0)
	{
		for (size_t i = 0; i < (size_t)numWindowEvents; i++)
		{
			const SDL_Event& sdlEvent = sdlEvents[i];

			switch (sdlEvent.type)
			{
			// Window events
			case SDL_WINDOWEVENT:
			{
				auto it = std::find_if(windows.begin(), windows.end(),
										[sdlEvent](const std::unique_ptr<WindowInternal>& _window)
										{
											return sdlEvent.window.windowID == _window->getID();
										});
				if (it == windows.end())
				{
					log::warning("Renderer: Window with ID " + std::to_string(sdlEvent.window.windowID) + " not found!");
					break;
				}
				WindowInternal& window = *it->get();
				if (window.wasDestroyed())
					break;
				window.handleWindowEvent(sdlEvent.window);
				break;
			}
			}
		}
	}
	else if (numWindowEvents == -1)
	{
		std::string error = "Renderer: SDL_PeepEvents failed! ";
		error += SDL_GetError();
		log::error(error);
	}
}
