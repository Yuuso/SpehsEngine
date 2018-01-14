#include "SpehsEngine/GUI/GUIRectangleRow.h"

namespace spehs
{
	GUIRectangleRow::GUIRectangleRow(GUIContext& context)
		: GUIRectangleContainer(context)
		, evenElementWidth(false)
		, elementPositionMode(PositionMode::Standard)
	{
		disableBit(state, GUIRECT_HOVER_COLOR_BIT);
	}

	GUIRectangleRow::~GUIRectangleRow()
	{
	}

	void GUIRectangleRow::setEvenElementWidth(const bool setting)
	{
		if (evenElementWidth == setting)
			return;
		evenElementWidth = setting;
		disableBit(state, GUIRECT_SCALE_UPDATED_BIT);
		disableBit(state, GUIRECT_POSITION_UPDATED_BIT);
	}

	void GUIRectangleRow::setElementPositionMode(const PositionMode mode)
	{
		if (elementPositionMode == mode)
			return;
		elementPositionMode = mode;
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
		disableBit(state, GUIRECT_POSITION_UPDATED_BIT);
	}

	void GUIRectangleRow::updateMinSize()
	{
		minSize.x = 0;
		minSize.y = 0;

		if (evenElementWidth)
		{//Uniform size
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i]->updateMinSize();
				if (elements[i]->getMinWidth() > minSize.x)
					minSize.x = elements[i]->getMinWidth();
				if (elements[i]->getMinHeight() > minSize.y)
					minSize.y = elements[i]->getMinHeight();
			}
			minSize.x *= elements.size();
		}
		else
		{//Dynamic size
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i]->updateMinSize();
				minSize.x += elements[i]->getMinWidth();
				if (elements[i]->getMinHeight() > minSize.y)
					minSize.y = elements[i]->getMinHeight();
			}
		}
		enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}

	void GUIRectangleRow::updateScale()
	{
		GUIRectangle::updateScale();

		if (elements.size() == 0)
			return;
		if (size == minSize)
		{//Set each element to it's minimum size
			if (evenElementWidth)
			{
				int w = 0;
				for (unsigned i = 0; i < elements.size(); i++)
				{
					if (elements[i]->getMinWidth() > w)
						w = elements[i]->getMinWidth();
				}
				for (unsigned i = 0; i < elements.size(); i++)
					elements[i]->setSize(w, minSize.y);
			}
			else
			{//Each to its own size
				for (unsigned i = 0; i < elements.size(); i++)
					elements[i]->setSize(elements[i]->getMinWidth(), minSize.y);
			}
		}
		else//size > min size
		{
			if (evenElementWidth)
			{
				const int w = size.x / elements.size();
				for (unsigned i = 0; i < elements.size() - 1; i++)
					elements[i]->setSize(w, size.y);
				elements.back()->setSize(size.x - w * (elements.size() - 1), size.y);
			}
			else if (elementPositionMode == PositionMode::StackLeft)
			{
				int allocatedWidth = 0;
				for (unsigned i = 0; i < elements.size(); i++)
				{
					if (i == elements.size() - 1)
					{//The last element
						elements[i]->setSize(size.x - allocatedWidth, size.y);
					}
					else
					{//Other elements get allocated their minimum size
						elements[i]->setSize(elements[i]->getMinWidth(), size.y);
						allocatedWidth += elements[i]->getMinWidth();
					}
				}
			}
			else if (elementPositionMode == PositionMode::StackRight)
			{
				int allocatedWidth = 0;
				for (int i = elements.size() - 1; i >= 0; i--)
				{
					if (i == 0)
					{//First element
						elements[i]->setSize(size.x - allocatedWidth, size.y);
					}
					else
					{//Other elements get allocated their min size
						elements[i]->setSize(elements[i]->getMinWidth(), size.y);
						allocatedWidth += elements[i]->getMinWidth();
					}
				}
			}
			else if (elementPositionMode == PositionMode::StackEdges)
			{
				if (elements.size() == 1)
				{//Only one element
					elements.front()->setSize(size.x, size.y);
				}
				else if (elements.size() == 2)
				{//Only edges
					int w1 = (float)elements.front()->getMinWidth() / ((float)elements.front()->getMinWidth() + (float)elements.back()->getMinWidth()) * size.x;
					elements.front()->setSize(w1, size.y);
					elements.back()->setSize(size.x - w1, size.y);
				}
				else
				{//Middle elements exist
					elements.front()->setSize(elements.front()->getMinWidth(), size.y);
					elements.back()->setSize(elements.back()->getMinWidth(), size.y);

					const int dynamicWidth = size.x - elements.front()->getMinWidth() - elements.back()->getMinWidth();
					const int dynamicMinWidth = minSize.x - elements.front()->getMinWidth() - elements.back()->getMinWidth();
					int allocatedWidth = 0;
					for (unsigned i = 1; i < elements.size() - 1; i++)
					{
						if (i == elements.size() - 2)
						{//The last dynamicly scaled element gets rest of the available width
							elements[i]->setSize(dynamicWidth - allocatedWidth, size.y);
						}
						else
						{
							const int w = (float)elements[i]->getMinWidth() / (float)dynamicMinWidth * dynamicWidth;
							elements[i]->setSize(w, size.y);
							allocatedWidth += w;
						}
					}
				}
			}
			else //PositionMode::Standard
			{//Scale each element relative to element min size : this->min size
				int allocatedWidth = 0;
				for (unsigned i = 0; i < elements.size() - 1; i++)
				{
					const int w = (float)elements[i]->getMinWidth() / (float)minSize.x * size.x;
					elements[i]->setSize(w, size.y);
					allocatedWidth += w;
				}
				elements.back()->setSize(size.x - allocatedWidth, size.y);
			}
		}
	}

	void GUIRectangleRow::updatePosition()
	{
		GUIRectangle::updatePosition();
		for (unsigned i = 0; i < elements.size(); i++)
		{
			if (i == 0)
				elements[i]->setPositionLocal(0, 0);
			else
				elements[i]->setPositionLocal(elements[i - 1]->getXLocal() + elements[i - 1]->getWidth(), 0);
		}
	}
}