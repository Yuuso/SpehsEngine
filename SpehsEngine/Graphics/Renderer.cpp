#include "stdafx.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"


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
			updateFlag(RendererFlag::MSAAX2, BGFX_RESET_MSAA_X2);
			updateFlag(RendererFlag::MSAAX4, BGFX_RESET_MSAA_X4);
			updateFlag(RendererFlag::MSAAX8, BGFX_RESET_MSAA_X8);
			updateFlag(RendererFlag::MSAAX16, BGFX_RESET_MSAA_X16);
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
				//init.type = bgfx::RendererType::OpenGL;
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

			if (rendererFlagsChanged)
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
				if (!windows[i]->render(renderContext))
				{
					windows[i].reset(windows.back().release());
					windows.pop_back();
					continue;
				}
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

		const RendererFlagsType Renderer::getRenderOptions() const
		{
			return rendererFlags;
		}
		const bool Renderer::checkRenderOption(const RendererFlag _renderFlag) const
		{
			return checkBit(rendererFlags, _renderFlag);
		}
		void Renderer::setRenderOptions(const RendererFlagsType _rendererFlags)
		{
			rendererFlags = _rendererFlags;
			rendererFlagsChanged = true;
		}
		void Renderer::enableRenderOption(const RendererFlag _renderFlag)
		{
			enableBit(rendererFlags, _renderFlag);
			rendererFlagsChanged = true;
		}
		void Renderer::disableRenderOption(const RendererFlag _renderFlag)
		{
			disableBit(rendererFlags, _renderFlag);
			rendererFlagsChanged = true;
		}

		void Renderer::debugTextPrintf(const uint16_t _column, const uint16_t _line, const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			bgfx::dbgTextPrintfVargs(_column, _line, 0x0f, _format, args);
			va_end(args);
		}
	}
}
