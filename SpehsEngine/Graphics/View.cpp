#include "stdafx.h"
#include "SpehsEngine/Graphics/View.h"


namespace se
{
	namespace graphics
	{
		View::View(Scene& _scene, Camera& _camera)
			: scene(_scene)
			, camera(_camera)
		{
		}
		View::~View()
		{
			destroyedSignal();
		}

		const ViewSize& View::getSize() const
		{
			return size;
		}
		const ViewSize& View::getOffset() const
		{
			return offset;
		}
		void View::setSize(const ViewSize& _size)
		{
			size = _size;
		}
		void View::setOffset(const ViewSize& _offset)
		{
			offset = _offset;
		}
	}
}
