#include "GUIRectangleTable.h"

namespace spehs
{
	GUIRectangleTable::GUIRectangleTable(BatchManager& _batchManager, const int _columns)
		: GUIRectangleUnisizeContainer(_batchManager)
		, columns(_columns)
	{
		disableBit(state, GUIRECT_HOVER_COLOR_BIT);
	}
	GUIRectangleTable::~GUIRectangleTable()
	{
	}
	void GUIRectangleTable::updateMinSize()
	{
		GUIRectangleUnisizeContainer::updateMinSize();

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
	void GUIRectangleTable::updatePosition()
	{
		GUIRectangle::updatePosition();

		int row = 0;
		int col = 0;
		int totalRowCount = ceil(float(elements.size()) / float(columns));
		for (unsigned i = 0; i < elements.size(); i++)
		{
			elements[i]->setPositionLocal(col * elementSize.x, elementSize.y * (totalRowCount - row - 1));
			col++;
			if (col >= columns)
			{
				col = 0;
				row++;
			}
		}
	}
}