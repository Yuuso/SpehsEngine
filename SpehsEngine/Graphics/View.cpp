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

		Scene& View::getScene()
		{
			return scene;
		}
		Camera& View::getCamera()
		{
			return camera;
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
		const uint32_t View::getClearColor() const
		{
			return clearColor;
		}
		const ViewClearFlagsType View::getClearFlags() const
		{
			return clearFlags;
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
		void View::setClearColor(const uint32_t _hexColor)
		{
			clearColor = _hexColor;
		}
		void View::setClearFlags(const ViewClearFlagsType _flags)
		{
			clearFlags = _flags;
		}

		boost::signals2::scoped_connection View::connectToOnRenderSignal(std::function<void(glm::vec2)> _func)
		{
			return onRenderSignal.connect(_func);
		}
	}
}
