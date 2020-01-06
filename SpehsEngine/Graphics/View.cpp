#include "stdafx.h"
#include "SpehsEngine/Graphics/View.h"


namespace se
{
	namespace graphics
	{
		View::View(const Scene& _scene, const Camera& _camera)
			: width(800)
			, height(600)
			, scene(_scene)
			, camera(_camera)
		{
		}
		View::~View()
		{
		}

		float View::getWidth() const
		{
			return width;
		}
		float View::getHeight() const
		{
			return height;
		}
	}
}
