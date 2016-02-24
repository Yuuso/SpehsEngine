#include "GUIRectangleTable.h"

namespace SpehsEngine
{
	GUIRectangleTable::GUIRectangleTable(int _columns) : columns(_columns)
	{
		disableBit(state, GUIRECT_HOVER_COLOR);
	}
	GUIRectangleTable::~GUIRectangleTable()
	{
	}
	void GUIRectangleTable::updatePosition()
	{
		GUIRectangle::updatePosition();

		int row = 0;
		int col = 0;
		int totalRowCount = ceil(float(elements.size()) / float(columns));
		for (unsigned i = 0; i < elements.size(); i++)
		{
			elements[i]->setPosition(col * elementSize.x, elementSize.y * (totalRowCount - row - 1));
			col++;
			if (col >= columns)
			{
				col = 0;
				row++;
			}
		}
	}
	void GUIRectangleTable::updateScale()
	{
		GUIRectangle::updateScale();
		float xScaleFactor = 0;
		float yScaleFactor = 0;
		if (elements.size() > 0 && columns > 0)
		{
			//X
			if (columns <= elements.size())
				xScaleFactor = size.x / float(minElementSize.x * columns);
			else
				xScaleFactor = size.x / float(minElementSize.x * elements.size());
			//Y
			yScaleFactor = size.y / float(minElementSize.y * ceil(float(elements.size()) / float(columns)));

			elementSize.x = floor(minElementSize.x * xScaleFactor);
			elementSize.y = floor(minElementSize.y * yScaleFactor);
			int row = 0;
			int col = 0;
			//int w = 0, h = 0;
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i]->setSize(elementSize);
				col++;
				if (col >= columns)
				{
					col = 0;
					row++;
				}
			}
		}
	}
	void GUIRectangleTable::updateMinSize()
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
		if (columns > 0)
		{
			if (elements.size() >= columns)
				minSize.x = minElementSize.x * columns;
			else
				minSize.x = minElementSize.x * elements.size();
			minSize.y = minElementSize.y * ceil(elements.size() / columns);
		}
		else
		{
			minSize.x = 0;
			minSize.y = 0;
		}
	}
}