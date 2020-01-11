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
		const bool View::getMSAAEnabled() const
		{
			return enableMSAA;
		}

		void View::setSize(const ViewSize& _size)
		{
			size = _size;
		}
		void View::setOffset(const ViewSize& _offset)
		{
			offset = _offset;
		}
		void View::setMSAAEnabled(const bool _enabled)
		{
			enableMSAA = _enabled;
		}
	}
}
