#include <algorithm>
#include "InputManager.h"
#include "GUIRectangleGrid.h"
#include "GUIRectangleScrollList.h"

namespace spehs
{
	GUIRectangleGrid::GUIRectangleGrid() : scrollingEnabled(false), scrollBarWidth(spehs::GUIRectangleScrollList::defaultScrollBarWidth),
		scrollState(0), draggingScrollBar(false), scrollBarAccumulatedDragY(0.0f)
	{
		disableState(GUIRECT_HOVER_COLOR_BIT);
		scrollUp = new spehs::GUIRectangle();
		scrollBar = new spehs::GUIRectangle();
		scrollDown = new spehs::GUIRectangle();
		scrollUp->setWidth(scrollBarWidth);
		scrollBar->setWidth(scrollBarWidth);
		scrollDown->setWidth(scrollBarWidth);
		scrollUp->setColor(GUIRectangleScrollList::defaultScrollElementColor);
		scrollBar->setColor(GUIRectangleScrollList::defaultScrollElementColor);
		scrollDown->setColor(GUIRectangleScrollList::defaultScrollElementColor);
		if (GUIRectangleScrollList::scrollUpTexturePath.size() > 0)
			scrollUp->setTexture(GUIRectangleScrollList::scrollUpTexturePath, defaultTextureParameters);
		else
			scrollUp->setString("U");
		if (GUIRectangleScrollList::scrollBarTexturePath.size() > 0)
			scrollBar->setTexture(GUIRectangleScrollList::scrollBarTexturePath, defaultTextureParameters);
		else
			scrollBar->setString("B");
		if (GUIRectangleScrollList::scrollDownTexturePath.size() > 0)
			scrollDown->setTexture(GUIRectangleScrollList::scrollDownTexturePath, defaultTextureParameters);
		else
			scrollDown->setString("D");
	}
	GUIRectangleGrid::~GUIRectangleGrid()
	{
		delete scrollUp;
		delete scrollBar;
		delete scrollDown;
	}
	void GUIRectangleGrid::setRenderState(const bool _state)
	{
		spehs::GUIRectangleUnisizeContainer::setRenderState(_state);
		scrollUp->setRenderState(scrollingEnabled && _state);
		scrollBar->setRenderState(scrollingEnabled && _state);
		scrollDown->setRenderState(scrollingEnabled && _state);
	}
	void GUIRectangleGrid::setDepth(const int16_t depth)
	{
		spehs::GUIRectangleUnisizeContainer::setDepth(depth);
		scrollUp->setDepth(depth + 1);
		scrollBar->setDepth(depth + 1);
		scrollDown->setDepth(depth + 1);
	}
	void GUIRectangleGrid::inputUpdate()
	{
		spehs::GUIRectangle::inputUpdate();
		
		if (draggingScrollBar)
		{//Dragging in progress?
			if (!inputManager->isKeyDown(MOUSEBUTTON_LEFT))
				draggingScrollBar = false;
			else
			{//Drag scroll bar
				scrollBarAccumulatedDragY += inputManager->getMouseMovementY();
				const float scrollBarRail = scrollUp->getYGlobal(/*cannot use local position - not actually a child!*/) - getYGlobal() - scrollDown->getHeight() - scrollBar->getHeight();
				const int maxStates = getMaxScrollState() + 1;
				if (abs(scrollBarAccumulatedDragY) > scrollBarRail / float(maxStates))
				{
					const int prevState = scrollState;
					scroll(-scrollBarAccumulatedDragY / (scrollBarRail / (float)maxStates));
					scrollBarAccumulatedDragY += int((scrollBarRail / (float)maxStates) * (scrollState - prevState));
				}
			}
		}

		if (isOpen())
		{
			if (scrollingEnabled)
			{
				scrollUp->inputUpdate();
				scrollBar->inputUpdate();
				scrollDown->inputUpdate();
				if (getMouseHover())
				{
					if (inputManager->isKeyPressed(MOUSEBUTTON_LEFT))
					{
						if (scrollUp->getMouseHover())
							scroll(-1);
						else if (scrollDown->getMouseHover())
							scroll(1);
						else if (scrollBar->getMouseHover())
						{
							draggingScrollBar = true;
							scrollBarAccumulatedDragY = 0.0f;
						}
					}
					if (inputManager->getMouseWheelDelta())
						scroll(-inputManager->getMouseWheelDelta());
				}
			}

			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (elements[i]->getRenderState())
					elements[i]->inputUpdate();
			}
		}
		else
		{//Is closed
			draggingScrollBar = false;
		}
	}
	void GUIRectangleGrid::visualUpdate()
	{
		spehs::GUIRectangle::visualUpdate();
		if (isOpen())
		{
			if (scrollingEnabled)
			{
				scrollUp->visualUpdate();
				scrollBar->visualUpdate();
				scrollDown->visualUpdate();
			}
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (elements[i]->getRenderState())
					elements[i]->visualUpdate();
			}
		}
	}
	int GUIRectangleGrid::getRowCount()
	{
		if (minElementSize.x <= 0 || elements.empty())
			return 0;
		const int wCount = (getWidth() - scrollBarWidth) / minElementSize.x;
		return std::ceil((float)elements.size() / (float)wCount);
	}
	int GUIRectangleGrid::getMaxScrollState()
	{
		return getRowCount() - getHeight() / minElementSize.y;
	}
	void GUIRectangleGrid::scroll(const int amount)
	{
		scrollState += amount;
		if (scrollState < 0)
			scrollState = 0;
		else
		{
			int maxScroll = getMaxScrollState();
			if (scrollState > maxScroll)
				scrollState = maxScroll;
		}
		disableState(GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIRectangleGrid::updateMinSize()
	{
		spehs::GUIRectangleUnisizeContainer::updateMinSize();
		minSize = minElementSize;
		if (elements.size() > 1)
			minSize.x += scrollBarWidth;
	}
	void GUIRectangleGrid::updateScale()
	{
		spehs::GUIRectangle::updateScale();
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->setSize(minElementSize);
		const int scrollHeight = std::min(minElementSize.y, getHeight() / 3);
		scrollUp->setHeight(scrollHeight);
		scrollBar->setHeight(scrollHeight / 2);
		scrollDown->setHeight(scrollHeight);
		scrollState = 0;
	}
	void GUIRectangleGrid::updatePosition()
	{
		spehs::GUIRectangle::updatePosition();
		if (minElementSize.x == 0 || minElementSize.y == 0)
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(false);
			scrollingEnabled = false;
			return;
		}
		const int hCount = getHeight() / minElementSize.y;
		scrollingEnabled = (/*Total elements possible to display*/getWidth() / minElementSize.x * hCount) < elements.size();
		const int relativeScrollBarWidth = scrollingEnabled ? scrollBarWidth : 0;
		const int wCount = (getWidth() - relativeScrollBarWidth) / minElementSize.x;
		if (wCount == 0 || hCount == 0)
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(false);
			return;
		}
		const int wInterval = (getWidth() - relativeScrollBarWidth - wCount * minElementSize.x) / (wCount + 1);
		const int hInterval = (getHeight() - hCount * minElementSize.y) / (hCount + 1);
		const int wExtra = getWidth() - relativeScrollBarWidth - wCount * minElementSize.x - wInterval * (1 + wCount);
		const int hExtra = getHeight() - hCount * minElementSize.y - hInterval * (1 + hCount);
		const int left = wInterval + relativeScrollBarWidth;
		int x = left;
		int y = getHeight() - hInterval - minElementSize.y;
		
		//Amount of "skipped" elements because of scrolling
		const int skipCount = wCount * scrollState;
		for (unsigned i = 0; i < skipCount; i++)
			elements[i]->setRenderState(false);
		for (unsigned i = skipCount; i < elements.size(); i++)
		{
			//Render state
			if (y < 0)
				elements[i]->setRenderState(false);
			else
				elements[i]->setRenderState(getRenderState());

			elements[i]->setPositionLocal(x, y);
			x += minElementSize.x + wInterval;
			if (x + minElementSize.x + wInterval + wExtra > getWidth())
			{//change row
				x = left;
				y -= hInterval + minElementSize.y;
			}
		}
		scrollUp->setRenderState(scrollingEnabled);
		scrollBar->setRenderState(scrollingEnabled);
		scrollDown->setRenderState(scrollingEnabled);
		if (scrollingEnabled)
		{
			scrollUp->setPositionLocal(getXGlobal(), getYGlobal() + getHeight() - scrollUp->getHeight());
			scrollDown->setPositionLocal(getXGlobal(), getYGlobal());
			const float barRailHeight = getHeight() - scrollUp->getHeight() - scrollBar->getHeight() - scrollDown->getHeight();
			const float percentage = (float)scrollState / (float)getMaxScrollState();
			scrollBar->setPositionLocal(getXGlobal(), getYGlobal() + scrollDown->getHeight() + (1.0f - percentage) * barRailHeight);
		}
	}
}