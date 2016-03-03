#include "GUIRectangleRow.h"

namespace spehs
{
	GUIRectangleRow::GUIRectangleRow() : evenElementWidth(false), elementPositionMode(PositionMode::Standard)
	{
		disableBit(state, GUIRECT_HOVER_COLOR);
	}
	GUIRectangleRow::~GUIRectangleRow()
	{
	}
	void GUIRectangleRow::setEvenElementWidth(bool setting)
	{
		if (evenElementWidth == setting)
			return;
		evenElementWidth = setting;
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_POSITIONED);
	}
	void GUIRectangleRow::setElementPositionMode(PositionMode mode)
	{
		if (elementPositionMode == mode)
			return;
		elementPositionMode = mode;
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_POSITIONED);
	}
	void GUIRectangleRow::updatePosition()
	{
		GUIRectangle::updatePosition();
		for (unsigned i = 0; i < elements.size(); i++)
		{
			if (i == 0)
				elements[i]->setPosition(0, 0);
			else
				elements[i]->setPosition(elements[i - 1]->getLocalX() + elements[i - 1]->getWidth(), 0);
		}

	}
	void GUIRectangleRow::updateScale()
	{
		GUIRectangle::updateScale();

		if (elements.size() == 0)
			return;
		if (size == minSize)
		{//Set each element to it's minimum size
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (evenElementWidth)
					elements[i]->setSize(minElementSize);
				else
					elements[i]->setSize(elements[i]->getMinWidth(), minSize.y);
			}
		}
		else //size > min size
		{
			int allocatedWidth, w;
			if (elementPositionMode == PositionMode::Standard || evenElementWidth)
			{//Scale each element scaling relative to element min size : this->min size
				allocatedWidth = 0;
				for (unsigned i = 1; i < elements.size(); i++)
				{
					if (evenElementWidth)
						w = minElementSize.x / float(minSize.x) * size.x;
					else
						w = elements[i]->getMinWidth() / float(minSize.x) * size.x;
					elements[i]->setSize(w, size.y);
					allocatedWidth += w;
				}
				elements.front()->setSize(size.x - allocatedWidth, size.y);//Allocate excess width to the first element
			}
			else if (elementPositionMode == PositionMode::Left)
			{
				allocatedWidth = 0;
				for (unsigned i = 0; i < elements.size(); i++)
				{
					if (i == elements.size() - 1)
						elements[i]->setSize(size.x - allocatedWidth, size.y);//Allocate excess width to the last element
					else
					{
						elements[i]->setSize(elements[i]->getMinWidth(), size.y);
						allocatedWidth += elements[i]->getMinWidth();
					}
				}
			}
			else if (elementPositionMode == PositionMode::Right)
			{
				allocatedWidth = 0;
				for (unsigned i = 1; i < elements.size(); i++)
				{
					elements[i]->setSize(elements[i]->getMinWidth(), size.y);
					allocatedWidth += elements[i]->getMinWidth();
				}
				if (elements.size() > 0)
					elements[0]->setSize(size.x - allocatedWidth, size.y);//Allocate excess width to the first element
			}
		}
	}
	void GUIRectangleRow::updateMinSize()
	{
		minSize.x = 0;
		minElementSize.x = 0;
		minElementSize.y = 0;
		for (unsigned i = 0; i < elements.size(); i++)
		{
			elements[i]->updateMinSize();
			if (elements[i]->getMinWidth() > minElementSize.x)
				minElementSize.x = elements[i]->getMinWidth();
			if (elements[i]->getMinHeight() > minElementSize.y)
				minElementSize.y = elements[i]->getMinHeight();

			//Increment min width
			if (!evenElementWidth)
				minSize.x += elements[i]->getMinWidth();
		}
		if (evenElementWidth)
			minSize.x = minElementSize.x * elements.size();

		//Set min height to highest min height found in elements
		minSize.y = minElementSize.y;
	}
}