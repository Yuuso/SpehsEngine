#include "stdafx.h"
#include "SpehsEngine/Graphics/View.h"


View::View(Scene& _scene, Camera& _camera)
	: scene(_scene)
	, camera(_camera)
{}
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
bool View::getMSAAEnabled() const
{
	return enableMSAA;
}
uint32_t View::getClearColor() const
{
	return clearColor;
}
ViewClearFlagsType View::getClearFlags() const
{
	return clearFlags;
}
DrawCallSortOrder View::getDrawCallSortOrder() const
{
	return drawCallSortOrder;
}

void View::setSize(const ViewSize& _size)
{
	size = _size;
}
void View::setOffset(const ViewSize& _offset)
{
	offset = _offset;
}
void View::setMSAAEnabled(bool _enabled)
{
	enableMSAA = _enabled;
}
void View::setClearColor(uint32_t _hexColor)
{
	clearColor = _hexColor;
}
void View::setClearFlags(ViewClearFlagsType _flags)
{
	clearFlags = _flags;
}
void View::setDrawCallSortOrder(DrawCallSortOrder _order)
{
	drawCallSortOrder = _order;
}

boost::signals2::scoped_connection View::connectToPreRenderSignal(std::function<void(glm::vec2)> _func)
{
	return onPreRenderSignal.connect(_func);
}
