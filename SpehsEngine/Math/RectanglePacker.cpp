#include "stdafx.h"
#include "SpehsEngine/Math/RectanglePacker.h"
#include "SpehsEngine/Core/SE_Assert.h"


namespace se
{
	RectanglePacker::Node::Node(const Rectangle& _rectangle)
		: rectangle(_rectangle)
	{
		// Discard thin slices
		//if (rectangle.width <= 2 || rectangle.height <= 2)
		//	filled = true;
	}
	bool RectanglePacker::Node::insert(Rectangle& _rectangle)
	{
		const bool isLeaf = children[0] == nullptr;

		if (!isLeaf)
		{
			const bool result = children[0]->insert(_rectangle);
			if (result)
				return true;
			return children[1]->insert(_rectangle);
		}

		if (filled)
			return false;
		if (rectangle.width < _rectangle.width || rectangle.height < _rectangle.height)
			return false;
		if (rectangle.width == _rectangle.width && rectangle.height == _rectangle.height)
		{
			_rectangle.x = rectangle.x;
			_rectangle.y = rectangle.y;
			filled = true;
			return true;
		}

		const uint16_t deltaWidth = rectangle.width - _rectangle.width;
		const uint16_t deltaHeight = rectangle.height - _rectangle.height;
		if (deltaWidth > deltaHeight)
		{
			// Split vertically
			children[0] = std::make_unique<Node>(Rectangle(	rectangle.x,						rectangle.y,	_rectangle.width,	rectangle.height));
			children[1] = std::make_unique<Node>(Rectangle(	rectangle.x + _rectangle.width,		rectangle.y,	deltaWidth,			rectangle.height));
		}
		else
		{
			// Split horizontally
			children[0] = std::make_unique<Node>(Rectangle(	rectangle.x,	rectangle.y,						rectangle.width,	_rectangle.height));
			children[1] = std::make_unique<Node>(Rectangle(	rectangle.x,	rectangle.y + _rectangle.height,	rectangle.width,	deltaHeight));
		}
		return children[0]->insert(_rectangle);
	}


	RectanglePacker::RectanglePacker(const uint16_t _width, const uint16_t _height)
		: rootNode(Rectangle(_width, _height))
		, width(_width)
		, height(_height)
	{
	}
	void RectanglePacker::clear()
	{
		rootNode.children[0].reset();
		rootNode.children[1].reset();
	}
	bool RectanglePacker::addRectangle(Rectangle& _rectangle)
	{
		return rootNode.insert(_rectangle);
	}
	uint16_t RectanglePacker::getWidth() const
	{
		return width;
	}
	uint16_t RectanglePacker::getHeight() const
	{
		return height;
	}
}
