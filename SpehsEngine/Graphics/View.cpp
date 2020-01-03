#include "stdafx.h"
#include "SpehsEngine/Graphics/View.h"


namespace se
{
	namespace graphics
	{
		View::View(const Scene& _scene, const Camera& _camera)
			: width(600)
			, height(600)
			, scene(_scene)
			, camera(_camera)
		{
		}
		View::~View()
		{
		}

		float View::widthGet() const
		{
			return width;
		}
		float View::heightGet() const
		{
			return height;
		}
	}
}
