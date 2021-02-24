#include "stdafx.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "SDL/SDL.h"
#include "SDL/SDL_events.h"
#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"


namespace se
{
	namespace graphics
	{
		Renderer::Renderer(Window& _window, const RendererFlagsType _rendererFlags, const RendererBackend _rendererBackend)
			: rendererFlags(_rendererFlags)
		{
			if (initialized)
			{
				se::log::error("You can only have one Renderer!");
				return;
			}
			initialized = true;

			windows.push_back(std::make_unique<WindowInstance>(_window, true));
			defaultWindow = windows.back().get();

			{
				bgfx::Init init;
				init.type = getRendererType(_rendererBackend);
				init.resolution.width = (uint32_t)_window.getWidth();
				init.resolution.height = (uint32_t)_window.getHeight();
				init.resolution.reset = getResetParameters(rendererFlags);
				//init.callback
				bgfx::init(init);

				bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);
				// TODO: BGFX_DEBUG_STATS

				bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x101010ff, 1.0f, 0);
			}

			{
				//const bgfx::Caps* caps = bgfx::getCaps();
				// TODO: Print infos...
			}

			defaultUniforms = std::make_unique<DefaultUniforms>();
		}
		Renderer::Renderer(Window& _window)
			: Renderer(_window, 0)
		{
		}
		Renderer::~Renderer()
		{
			se_assert(initialized);
			windows.clear();
			defaultUniforms.reset();
			bgfx::shutdown();
			while (bgfx::renderFrame() != bgfx::RenderFrame::NoContext) {}
			initialized = false;
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

			RenderContext renderContext;
			renderContext.rendererFlags = rendererFlags;
			renderContext.defaultUniforms = defaultUniforms.get();
			for (size_t i = 0; i < windows.size(); )
			{
				if (windows[i]->wasDestroyed())
				{
					windows[i].reset(windows.back().release());
					windows.pop_back();
					continue;
				}
				windows[i]->update();
				windows[i]->render(renderContext);
				i++;
			}
			bgfx::frame();
			bgfx::dbgTextClear();
		}

		void Renderer::add(Window& _window)
		{
			auto it = std::find_if(windows.begin(),
								   windows.end(),
								   [&_window](const std::unique_ptr<WindowInstance>& window)
								   {
									   return *window.get() == _window;
								   });
			if (it != windows.end())
			{
				se::log::error("Window already found in renderer!");
				return;
			}
			windows.push_back(std::make_unique<WindowInstance>(_window, false));
		}
		void Renderer::remove(Window& _window)
		{
			auto it = std::find_if(windows.begin(),
								   windows.end(),
								   [&_window](const std::unique_ptr<WindowInstance>& window)
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

		const RendererFlagsType Renderer::getRendererFlags() const
		{
			return rendererFlags;
		}
		const bool Renderer::checkRendererFlag(const RendererFlag _renderFlag) const
		{
			return checkBit(rendererFlags, _renderFlag);
		}
		void Renderer::setRendererFlags(const RendererFlagsType _rendererFlags)
		{
			rendererFlags = _rendererFlags;
			rendererFlagsChanged = true;
		}
		void Renderer::enableRendererFlag(const RendererFlag _renderFlag)
		{
			enableBit(rendererFlags, _renderFlag);
			rendererFlagsChanged = true;
		}
		void Renderer::disableRendererFlag(const RendererFlag _renderFlag)
		{
			disableBit(rendererFlags, _renderFlag);
			rendererFlagsChanged = true;
		}

		const RendererBackend Renderer::getRendererBackend()
		{
			se_assert(initialized);
			return graphics::getRendererBackend();
		}

		const glm::ivec2 Renderer::getDisplaySize()
		{
			se_assert(initialized);
			SDL_DisplayMode displayMode;
			if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
			{
				std::string error = "Renderer: Failed to get display size! ";
				error += SDL_GetError();
				log::error(error);
				return glm::ivec2(0);
			}
			return glm::ivec2(displayMode.w, displayMode.h);
		}

		const glm::vec2 Renderer::getDisplayDPI()
		{
			se_assert(initialized);
			glm::vec2 result;
			float unused = 0.0f;
			if (SDL_GetDisplayDPI(0, &unused, &result.x, &result.y) != 0)
			{
				std::string error = "Renderer: Failed to get display dpi! ";
				error += SDL_GetError();
				log::error(error);
			}
			return result;
		}

		const int Renderer::getDisplayRefreshRate()
		{
			se_assert(initialized);
			SDL_DisplayMode displayMode;
			if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
			{
				std::string error = "Renderer: Failed to get display refresh rate! ";
				error += SDL_GetError();
				log::error(error);
				return 0;
			}
			return displayMode.refresh_rate;
		}

		void Renderer::debugTextPrintf(const uint16_t _column, const uint16_t _line, const char* _format, ...)
		{
			se_assert(initialized);
			va_list args;
			va_start(args, _format);
			bgfx::dbgTextPrintfVargs(_column, _line, 0x0f, _format, args);
			va_end(args);
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
											   [sdlEvent](const std::unique_ptr<WindowInstance>& _window)
											   {
												   return sdlEvent.window.windowID == _window->getID();
											   });
						if (it == windows.end())
						{
							log::warning("Renderer: Window with ID " + std::to_string(sdlEvent.window.windowID) + " not found!");
							break;
						}
						WindowInstance& window = *it->get();
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
	}
}
