#pragma once

#include "bgfx/bgfx.h" // TODO


namespace se
{
	namespace graphics
	{
		struct RenderContext
		{
			const class WindowInstance* currentWindow = nullptr;
			bgfx::ViewId currentViewId = 0;
		};
	}
}
