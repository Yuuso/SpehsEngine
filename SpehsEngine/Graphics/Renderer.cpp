#include "stdafx.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "SDL/SDL.h"
#include "SDL/SDL_events.h"


namespace se
{
	namespace graphics
	{
		static bool initialized = false;

		static uint32_t getResetParameters(const RendererFlagsType _rendererFlags)
		{
			uint32_t result = BGFX_RESET_NONE;
			auto updateFlag =
				[&result, _rendererFlags](const RendererFlag _flag, const uint32_t _param) -> void
				{
					if (checkBit(_rendererFlags, _flag))
						result |= _param;
				};

			updateFlag(RendererFlag::VSync, BGFX_RESET_VSYNC);
			updateFlag(RendererFlag::MSAA2, BGFX_RESET_MSAA_X2);
			updateFlag(RendererFlag::MSAA4, BGFX_RESET_MSAA_X4);
			updateFlag(RendererFlag::MSAA8, BGFX_RESET_MSAA_X8);
			updateFlag(RendererFlag::MSAA16, BGFX_RESET_MSAA_X16);
			//updateFlag(RendererFlag::?, BGFX_RESET_MAXANISOTROPY);
			//updateFlag(RendererFlag::?, BGFX_RESET_FULLSCREEN);
			//updateFlag(RendererFlag::?, BGFX_RESET_SRGB_BACKBUFFER);
			//updateFlag(RendererFlag::?, BGFX_RESET_HDR10);
			//updateFlag(RendererFlag::?, BGFX_RESET_HIDPI);
			//updateFlag(RendererFlag::?, BGFX_RESET_DEPTH_CLAMP);
			//updateFlag(RendererFlag::?, BGFX_RESET_SUSPEND);
			//updateFlag(RendererFlag::?, BGFX_RESET_FLUSH_AFTER_RENDER);
			//updateFlag(RendererFlag::?, BGFX_RESET_FLIP_AFTER_RENDER);

			return result;
		}

		static bgfx::RendererType::Enum getRendererType(const RendererFlagsType _rendererFlags)
		{
			bgfx::RendererType::Enum result = bgfx::RendererType::Count;
			if (checkBit(_rendererFlags, RendererFlag::Direct3D9))
				result = bgfx::RendererType::Direct3D9;
			else if (checkBit(_rendererFlags, RendererFlag::Direct3D11))
				result = bgfx::RendererType::Direct3D11;
			else if (checkBit(_rendererFlags, RendererFlag::Direct3D12))
				result = bgfx::RendererType::Direct3D12;
			else if (checkBit(_rendererFlags, RendererFlag::Gnm))
				result = bgfx::RendererType::Gnm;
			else if (checkBit(_rendererFlags, RendererFlag::Metal))
				result = bgfx::RendererType::Metal;
			else if (checkBit(_rendererFlags, RendererFlag::Nvn))
				result = bgfx::RendererType::Nvn;
			else if (checkBit(_rendererFlags, RendererFlag::OpenGLES))
				result = bgfx::RendererType::OpenGLES;
			else if (checkBit(_rendererFlags, RendererFlag::OpenGL))
				result = bgfx::RendererType::OpenGL;
			else if (checkBit(_rendererFlags, RendererFlag::Vulkan))
				result = bgfx::RendererType::Vulkan;
			return result;
		}


		Renderer::Renderer(Window& _window, const RendererFlagsType _rendererFlags)
			: rendererFlags(_rendererFlags)
		{
			if (initialized)
			{
				se::log::error("You can only have one renderer.");
				return;
			}
			initialized = true;

			windows.emplace_back(std::make_unique<WindowInstance>(_window, true));
			defaultWindow = windows.back().get();

			{
				bgfx::Init init;
				init.type = getRendererType(rendererFlags);
				init.resolution.width = (uint32_t)_window.getWidth();
				init.resolution.height = (uint32_t)_window.getHeight();
				init.resolution.reset = getResetParameters(rendererFlags);
				//init.callback
				bgfx::init(init);

				bgfx::setDebug(BGFX_DEBUG_TEXT);
				// TODO: BGFX_DEBUG_STATS

				bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x101010ff, 1.0f, 0);
			}

			{
				//const bgfx::Caps* caps = bgfx::getCaps();
				// TODO: Print infos...
			}
		}
		Renderer::Renderer(Window& _window)
			: Renderer(_window, 0)
		{
		}
		Renderer::~Renderer()
		{
			windows.clear();
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
			windows.emplace_back(std::make_unique<WindowInstance>(_window, false));
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

		const glm::ivec2 Renderer::getDisplaySize() const
		{
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
		const int Renderer::getDisplayRefreshRate() const
		{
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
