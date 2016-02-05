#include <iostream>
#include "GUIRectangleGrid.h"
#include "GUIRectangleRow.h"
#include "ApplicationData.h"
#define MIN_VISIBLE_COUNT 6

namespace SpehsEngine
{
	GUIRectangleGrid::GUIRectangleGrid() : squareShaped(false)
	{
		disableBit(state, GUIRECT_HOVER_COLOR);
		columnCount = 1;
		rowCount = 1;
	}
	GUIRectangleGrid::~GUIRectangleGrid()
	{
	}
	void GUIRectangleGrid::clear()
	{
		GUIRectangleContainer::clear();
		rowCount = 1;
		columnCount = 1;
	}
	void GUIRectangleGrid::updateGridDimensions()
	{
		while ((columnCount + 1) * minElementSize.x < size.x)
		{
			columnCount++;
		}
		while (columnCount * minElementSize.x <= size.x)
		{
			columnCount--;
		}
	}
	void GUIRectangleGrid::updateScale()
	{
		updateGridDimensions();
		GUIRectangle::updateScale();
		elementSize.x = size.x / columnCount;
		elementSize.y = size.y / rowCount;

		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->setSize(elementSize);

	}
	void GUIRectangleGrid::updatePosition()
	{
		GUIRectangle::updatePosition();

		int ele = 0;
		for (unsigned row = 0; row < rowCount; row++)
		{
			for (unsigned col = 0; col < columnCount; col++)
			{
				if (ele == elements.size())
					return;
				elements[ele]->setPosition(col * elementSize.x, row * elementSize.y);
				ele++;
			}
		}
	}
	void GUIRectangleGrid::updateMinSize()
	{
		minElementSize.x = 0;
		minElementSize.y = 0;

		for (unsigned i = 0; i < elements.size(); i++)
		{
			elements[i]->updateMinSize();
			if (elements[i]->getMinWidth() > minElementSize.x)
				minElementSize.x = elements[i]->getMinWidth();
			if (elements[i]->getMinHeight() > minElementSize.y)
				minElementSize.y = elements[i]->getMinHeight();
		}

		//Make min size square if so requested
		if (squareShaped)
		{
			if (minElementSize.x > minElementSize.y)
				minElementSize.y = minElementSize.x;
			else
				minElementSize.x = minElementSize.y;
		}

		minSize.x = minElementSize.x * columnCount;
		minSize.y = minElementSize.y * rowCount;
	}
}