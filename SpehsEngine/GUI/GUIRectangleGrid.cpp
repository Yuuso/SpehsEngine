#include <algorithm>
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/GUI/GUIRectangleGrid.h"
#include "SpehsEngine/GUI/GUIRectangleScrollList.h"

namespace se
{
	unsigned GUIRectangleGrid::defaultBorderWidth = 0;
	GUIRectangleGrid::GUIRectangleGrid(GUIContext& context)
		: GUIRectangleUnisizeContainer(context)
		, scrollBarWidth(se::GUIRectangleScrollList::defaultScrollBarWidth)
		, borderWidth(defaultBorderWidth)
	{
		disableState(GUIRECT_HOVER_COLOR_BIT);
		scrollUp = new se::GUIRectangle(context);
		scrollBar = new se::GUIRectangle(context);
		scrollDown = new se::GUIRectangle(context);
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
		se::GUIRectangleUnisizeContainer::setRenderState(_state);
		scrollUp->setRenderState(scrollingEnabled && _state);
		scrollBar->setRenderState(scrollingEnabled && _state);
		scrollDown->setRenderState(scrollingEnabled && _state);
	}

	void GUIRectangleGrid::setDepth(const int16_t depth)
	{
		se::GUIRectangleUnisizeContainer::setDepth(depth);
		scrollUp->setDepth(depth + 1);
		scrollBar->setDepth(depth + 1);
		scrollDown->setDepth(depth + 1);
	}

	void GUIRectangleGrid::inputUpdate()
	{
		se::GUIRectangle::inputUpdate();
		
		if (draggingScrollBar)
		{//Dragging in progress?
			if (!getInputManager().isKeyDown(MOUSEBUTTON_LEFT))
				draggingScrollBar = false;
			else
			{//Drag scroll bar
				scrollBarAccumulatedDragY += getInputManager().getMouseMovementY();
				const float scrollBarRail = float(scrollUp->getYGlobal(/*cannot use local position - not actually a child!*/) - getYGlobal() - scrollDown->getHeight() - scrollBar->getHeight());
				const int maxStates = getMaxScrollState() + 1;
				if (abs(scrollBarAccumulatedDragY) > scrollBarRail / float(maxStates))
				{
					const int prevState = scrollState;
					scroll(int(-scrollBarAccumulatedDragY / (scrollBarRail / (float)maxStates)));
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
					if (getInputManager().isKeyPressed(MOUSEBUTTON_LEFT))
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
					if (getInputManager().getMouseWheelDelta())
						scroll(-getInputManager().getMouseWheelDelta());
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
		se::GUIRectangle::visualUpdate();
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

	int GUIRectangleGrid::getPreferredHeight()
	{
		return getMinHeight() * getRowCount() + 2 * borderWidth;
	}

	int GUIRectangleGrid::getRowCount()
	{
		if (minElementSize.x <= 0 || elements.empty())
			return 0;
		const int wCount = (getWidth() - 2 * borderWidth - scrollBarWidth) / minElementSize.x;
		return int(std::ceil(float(elements.size()) / float(wCount)));
	}

	int GUIRectangleGrid::getMaxScrollState()
	{
		if (minElementSize.y == 0)
			return 0;
		return std::max(0, getRowCount() - (getHeight() - 2 * (int)borderWidth) / minElementSize.y);
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
		se::GUIRectangleUnisizeContainer::updateMinSize();
		minSize = minElementSize;
		minSize.x += 2 * borderWidth;
		minSize.y += 2 * borderWidth;
		if (elements.size() > 1)
			minSize.x += scrollBarWidth;
	}

	void GUIRectangleGrid::updateScale()
	{
		se::GUIRectangle::updateScale();
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->setSize(minElementSize);
		const int scrollHeight = std::min(minElementSize.y, getHeight() / 3);
		scrollUp->setHeight(scrollHeight);
		scrollBar->setHeight(scrollHeight / 2);
		scrollDown->setHeight(scrollHeight);
		const int maxScrollState = getMaxScrollState();
		if (scrollState > maxScrollState)
			scrollState = maxScrollState;
	}

	void GUIRectangleGrid::updatePosition()
	{
		se::GUIRectangle::updatePosition();
		if (minElementSize.x == 0 || minElementSize.y == 0)
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(false);
			scrollingEnabled = false;
			return;
		}
		const int hCount = (getHeight() - 2 * borderWidth) / minElementSize.y;
		scrollingEnabled = ((getWidth() - 2 * borderWidth) / minElementSize.x * hCount)/*Total elements possible to display*/ < elements.size();
		const int relativeScrollBarWidth = scrollingEnabled ? scrollBarWidth : 0;
		const int wCount = (getWidth() - relativeScrollBarWidth - 2 * borderWidth) / minElementSize.x;
		if (wCount == 0 || hCount == 0)
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(false);
			return;
		}
		const int wInterval = (getWidth() - relativeScrollBarWidth - wCount * minElementSize.x - 2 * borderWidth) / (wCount + 1);
		const int hInterval = (getHeight() - hCount * minElementSize.y - 2 * borderWidth) / (hCount + 1);
		const int wExtra = getWidth() - relativeScrollBarWidth - wCount * minElementSize.x - wInterval * (1 + wCount) - 2 * borderWidth;//Extra width to be allocated at the back
		const int hExtra = getHeight() - hCount * minElementSize.y - hInterval * (1 + hCount) - 2 * borderWidth;//Extra height to be allocated to the back
		const int left = wInterval + relativeScrollBarWidth + borderWidth;
		int x = left;
		int y = getHeight() - hInterval - minElementSize.y - borderWidth;
		
		//Amount of "skipped" elements because of scrolling
		const unsigned skipCount = wCount * scrollState;
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
			if (x + minElementSize.x + wInterval + wExtra + int(borderWidth) > getWidth())
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
			scrollUp->setPositionLocal(getXGlobal() + borderWidth, getYGlobal() + getHeight() - scrollUp->getHeight() - borderWidth);
			scrollDown->setPositionLocal(getXGlobal() + borderWidth, getYGlobal() + borderWidth);
			const float barRailHeight = float(getHeight() - scrollUp->getHeight() - scrollBar->getHeight() - scrollDown->getHeight() - 2 * borderWidth);
			const float percentage = float(scrollState) / float(getMaxScrollState());
			scrollBar->setPositionLocal(getXGlobal() + borderWidth, scrollDown->getYLocal() + scrollDown->getHeight() + int((1.0f - percentage) * barRailHeight));
		}
	}

	void GUIRectangleGrid::setGridBorderWidth(const unsigned width)
	{
		borderWidth = width;
		disableState(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
	}
}