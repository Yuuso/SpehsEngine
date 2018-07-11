#include "SpehsEngine/GUI/GUIRectangleTable.h"

namespace spehs
{
	GUIRectangleTable::GUIRectangleTable(GUIContext& context, const int _columns)
		: GUIRectangleUnisizeContainer(context)
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

		if (columns > 0u)
		{
			if (elements.size() >= columns)
				minSize.x = minElementSize.x * int(columns);
			else
				minSize.x = minElementSize.x * int(elements.size());
			minSize.y = minElementSize.y * (int)ceil(elements.size() / columns);
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

			elementSize.x = (int)floor((float)minElementSize.x * xScaleFactor);
			elementSize.y = (int)floor((float)minElementSize.y * yScaleFactor);
			size_t row = 0;
			size_t col = 0;
			//int w = 0, h = 0;
			for (size_t i = 0; i < elements.size(); i++)
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
		size_t col = 0;
		const size_t totalRowCount = (size_t)ceil(float(elements.size()) / float(columns));
		for (size_t i = 0; i < elements.size(); i++)
		{
			elements[i]->setPositionLocal(int(col * elementSize.x), int(elementSize.y * (totalRowCount - row - 1)));
			col++;
			if (col >= columns)
			{
				col = 0;
				row++;
			}
		}
	}
}