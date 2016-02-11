#include "GUIRectangleRow.h"

namespace SpehsEngine
{
	GUIRectangleRow::GUIRectangleRow() : evenElementWidth(false)
	{
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

		if (size == minSize)
		{//Set each element to it's minimum size
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (evenElementWidth)
					elements[i]->setSize(minElementSize);
				else
					elements[i]->setSize(elements[i]->getMinSize());
			}
		}
		else //size > min size
		{//Scale each element scaling relative to element min size : this->min size
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (evenElementWidth)
					elements[i]->setSize(minElementSize.x / float(minSize.x) * size.x, size.y);
				else
					elements[i]->setSize(elements[i]->getMinWidth() / float(minSize.x) * size.x, size.y);
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