#include "stdafx.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Log.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"


namespace se
{
	namespace graphics
	{
		static bool initialized = false;

		Renderer::Renderer(const Window& _window)
		{
			if (initialized)
			{
				se::log::error("You can only have one renderer.");
				return;
			}
			initialized = true;

			windows.emplace_back(std::make_unique<WindowInstance>(_window, true));

			{
				bgfx::Init init;
				//init.type = bgfx::RendererType::OpenGL;
				init.resolution.width = (uint32_t)_window.widthGet();
				init.resolution.height = (uint32_t)_window.heightGet();
				init.resolution.reset = BGFX_RESET_VSYNC;
				bgfx::init(init);

				bgfx::setDebug(BGFX_DEBUG_TEXT);

				bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x101010ff, 1.0f, 0);
			}

			{
				//const bgfx::Caps* caps = bgfx::getCaps();
				// TODO: Print infos...
			}
		}
		Renderer::~Renderer()
		{
			bgfx::shutdown();
			while (bgfx::renderFrame() != bgfx::RenderFrame::NoContext) {}
			initialized = false;
		}

		void Renderer::render()
		{
			for (auto&& window : windows)
			{
				window->render();
			}
			bgfx::frame();
			bgfx::dbgTextClear();
		}

		void Renderer::add(const Window& _window)
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
		void Renderer::remove(const Window& _window)
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
			if (it->get()->isDefault)
			{
				se::log::error("Cannot remove default window!");
				return;
			}
			windows.erase(it);
		}

		void Renderer::debugTextPrintf(const uint16_t _x, const uint16_t _y, const char* _format, ...)
		{
			va_list args;
			va_start(args, _format);
			bgfx::dbgTextPrintfVargs(_x, _y, 0x0f, _format, args);
			va_end(args);
		}
	}
}
