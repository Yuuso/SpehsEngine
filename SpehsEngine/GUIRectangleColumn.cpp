#include "GUIRectangleColumn.h"


namespace spehs
{
	GUIRectangleColumn::GUIRectangleColumn(const bool _uniformHeight) : uniformHeight(_uniformHeight)
	{

	}
	GUIRectangleColumn::~GUIRectangleColumn()
	{

	}
	void GUIRectangleColumn::setUniformHeight(const bool _state)
	{
		if (_state == uniformHeight)
			return;
		uniformHeight = _state;
		disableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIRectangleColumn::updateMinSize()
	{
		minSize.x = 0;
		minSize.y = 0;
		if (uniformHeight)
		{//Uniform height spacing
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i]->updateMinSize();
				if (elements[i]->getMinWidth() > minSize.x)
					minSize.x = elements[i]->getMinWidth();
				if (elements[i]->getMinHeight() > minSize.y)
					minSize.y = elements[i]->getMinHeight();
			}
			minSize.y *= elements.size();
		}
		else
		{
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i]->updateMinSize();
				if (elements[i]->getMinWidth() > minSize.x)
					minSize.x = elements[i]->getMinWidth();
				minSize.y += elements[i]->getMinHeight();
			}
		}
		enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIRectangleColumn::updateScale()
	{
		GUIRectangle::updateScale();

		if (elements.size() > 0)
		{
			if (uniformHeight)
			{//Uniform height spacing
				const int heightPerElement = size.y / elements.size();
				for (unsigned i = 0; i < elements.size() - 1; i++)
					elements[i]->setSize(size.x, heightPerElement);
				elements.back()->setSize(size.x, size.y - heightPerElement * elements.size());
			}
			else
			{
				int heightAllocated = 0;
				for (unsigned i = 0; i < elements.size() - 1; i++)
				{
					const int h = (float)elements[i]->getMinHeight() / (float)minSize.y * size.y;
					elements[i]->setSize(size.x, h);
					heightAllocated += h;
				}
				elements.back()->setSize(size.x, size.y - heightAllocated);
			}
		}
	}
	void GUIRectangleColumn::updatePosition()
	{
		GUIRectangle::updatePosition();

		int y = size.y;
		for (unsigned i = 0; i < elements.size(); i++)
		{
			y -= elements[i]->getHeight();
			elements[i]->setPositionLocal(0, y);
		}
	}
}