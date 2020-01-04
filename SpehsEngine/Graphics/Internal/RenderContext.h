#pragma once

#include "bgfx/bgfx.h" // TODO


namespace se
{
	namespace graphics
	{
		struct RenderContext
		{
			bgfx::ViewId currentViewId = 0;
		};
	}
}
