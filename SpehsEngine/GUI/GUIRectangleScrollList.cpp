#include "stdafx.h"
#include <algorithm>
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/GUI/GUIRectangleScrollList.h"
#include "SpehsEngine/GUI/GUIWindow.h"
#include "SpehsEngine/GUI/GUIStringEditor.h"
#include "SpehsEngine/GUI/GUIRectangle.h"


namespace se
{
	namespace ScrollButtons{ enum ScrollButtons{ up, bar, down }; }
	int GUIRectangleScrollList::defaultScrollBarWidth = 20;
	se::Color GUIRectangleScrollList::defaultScrollElementColor(255, 255, 255, 255);
	std::string GUIRectangleScrollList::scrollUpTexturePath;
	std::string GUIRectangleScrollList::scrollBarTexturePath;
	std::string GUIRectangleScrollList::scrollDownTexturePath;
	GUIRectangleScrollList::GUIRectangleScrollList(GUIContext& context)
		: GUIRectangleUnisizeContainer(context)
	{
		disableState(GUIRECT_HOVER_COLOR_BIT);
		scrollUp = new GUIRectangle(context);
		scrollUp->setID(ScrollButtons::up);
		scrollBar = new GUIRectangle(context);
		scrollBar->setID(ScrollButtons::bar);
		scrollDown = new GUIRectangle(context);
		scrollDown->setID(ScrollButtons::down);
		scrollBar->setWidth(defaultScrollBarWidth);
		scrollUp->setParent(this);
		scrollBar->setParent(this);
		scrollDown->setParent(this);
		if (scrollUpTexturePath.size() > 0)
			scrollUp->setTexture(scrollUpTexturePath, defaultTextureParameters);
		else
			scrollUp->setString("U");
		if (scrollBarTexturePath.size() > 0)
			scrollBar->setTexture(scrollBarTexturePath, defaultTextureParameters);
		else
			scrollBar->setString("B");
		if (scrollDownTexturePath.size() > 0)
			scrollDown->setTexture(scrollDownTexturePath, defaultTextureParameters);
		else
			scrollDown->setString("D");
		scrollUp->setColor(defaultScrollElementColor);
		scrollBar->setColor(defaultScrollElementColor);
		scrollDown->setColor(defaultScrollElementColor);
		scrollUp->setRenderState(false);
		scrollBar->setRenderState(false);
		scrollDown->setRenderState(false);
		setDepth(getDepth());
	}

	GUIRectangleScrollList::~GUIRectangleScrollList()
	{
		delete scrollUp;
		delete scrollBar;
		delete scrollDown;
	}

	void GUIRectangleScrollList::clear()
	{
		GUIRectangleContainer::clear();
		beginElementIndex = 0u;
		updateElementCount = 0u;
	}

	bool GUIRectangleScrollList::open()
	{
		if (!GUIRectangleUnisizeContainer::open())
			return false;
		scrollUp->setRenderState(invisibleElements());
		scrollBar->setRenderState(invisibleElements());
		scrollDown->setRenderState(invisibleElements());
		return true;
	}

	bool GUIRectangleScrollList::close()
	{
		if (!GUIRectangleUnisizeContainer::close())
			return false;
		scrollUp->setRenderState(false);
		scrollBar->setRenderState(false);
		scrollDown->setRenderState(false);
		return true;
	}

	bool GUIRectangleScrollList::removeElement(GUIRectangle* element)
	{
		for (size_t i = 0; i < elements.size(); i++)
		{
			if (elements[i] == element)
			{
				if (beginElementIndex + updateElementCount >= elements.size())
				{//Must decrease begin element index or visible element count
					if (beginElementIndex > 0)
					{//First visible element index decreases
						--beginElementIndex;
						elements[beginElementIndex]->setRenderState(getRenderState() && checkState(GUIRECT_OPEN_BIT));
					}
					else
					{//Visible element count decreases
						--updateElementCount;
					}
					disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
				}
				else
				{//Next element in the list becomes visible
					elements[beginElementIndex + updateElementCount]->setRenderState(getRenderState() && checkState(GUIRECT_OPEN_BIT));
				}
				delete elements[i];
				elements.erase(elements.begin() + i);
				disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
				return true;
			}
		}
		return false;
	}

	int GUIRectangleScrollList::getPreferredHeight()
	{
		if (!checkState(GUIRECT_MIN_SIZE_UPDATED_BIT))
			updateMinSize();
		return minElementSize.y * int(elements.size());
	}

	void GUIRectangleScrollList::inputUpdate()
	{
		processQueuedElementRemovals();

		if (getInputEnabled() && checkState(GUIRECT_OPEN_BIT))
		{
			scrollUp->inputUpdate();
			scrollBar->inputUpdate();
			scrollDown->inputUpdate();

			if (getInputManager().isKeyDown(MOUSE_BUTTON_LEFT) && //Mouse left is held
				getInputManager().getMouseY() > scrollDown->getYGlobal() + scrollDown->getHeight() && //mouse is within scroll bar area of movement
				getInputManager().getMouseY() < scrollUp->getYGlobal() &&
				getInputManager().getMouseX() > scrollDown->getXGlobal() &&
				getInputManager().getMouseX() < scrollDown->getXGlobal() + scrollDown->getWidth())
			{//Mouse dragging scroll bar

				//Calculate scroll based on mouse position relative to scroll area
				const float scrollAreaBegin = float(scrollDown->getYGlobal() + scrollDown->getHeight() + scrollBar->getHeight()) * 0.5f;//Y value of the scrolling area beginning
				const float scrollAreaHeight = float(scrollUp->getYGlobal() - scrollDown->getYGlobal() - scrollDown->getHeight() - scrollBar->getHeight());//Height of the scrolling area in between min and max positions of scroll bar center
				scroll(int(round((elements.size() - updateElementCount) * (1.0f - (getInputManager().getMouseY() - scrollAreaBegin) / scrollAreaHeight)) - beginElementIndex));
			}

			if (getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT) && invisibleElements())
			{
				if (scrollUp->getMouseHover())
					scroll(-1);
				else if (scrollDown->getMouseHover())
					scroll(1);
			}
			if (getMouseHover() && getInputManager().getMouseWheelDelta())
				scroll(-getInputManager().getMouseWheelDelta());
		}

		disableState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT);
		GUIRectangle::inputUpdate();
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (size_t i = 0; i < updateElementCount; i++)
			{
				elements[beginElementIndex + i]->inputUpdate();
				if (elements[beginElementIndex + i]->getMouseHoverAny())
					enableState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT);
			}
		}
	}

	void GUIRectangleScrollList::visualUpdate()
	{
		GUIRectangleContainer::visualUpdate();
		if (checkState(GUIRECT_OPEN_BIT))
		{
			scrollUp->visualUpdate();
			scrollBar->visualUpdate();
			scrollDown->visualUpdate();
		}
	}

	void GUIRectangleScrollList::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (size_t i = 0; i < elements.size(); i++)
			{
				if (i < beginElementIndex || i >= beginElementIndex + updateElementCount)
					elements[i]->setRenderState(false);
				else
					elements[i]->setRenderState(_state);
			}
			scrollUp->setRenderState(invisibleElements() && _state);
			scrollBar->setRenderState(invisibleElements() && _state);
			scrollDown->setRenderState(invisibleElements() && _state);
		}
		else
		{//Not open
			for (size_t i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(false);
			scrollUp->setRenderState(false);
			scrollBar->setRenderState(false);
			scrollDown->setRenderState(false);
		}
	}

	void GUIRectangleScrollList::updateMinSize()
	{
		if (checkState(GUIRECT_OPEN_BIT))
		{
			GUIRectangleUnisizeContainer::updateMinSize();

			minSize.x = minElementSize.x;
			minSize.y = minElementSize.y * int(std::min(minVisibleElementCount, updateElementCount));

			//Account scroll button width into min width
			if (invisibleElements())
				minSize.x += scrollBar->getWidth();
		}
		else
		{
			minSize.x = 0;
			minSize.y = 0;
			enableState(GUIRECT_MIN_SIZE_UPDATED_BIT);
		}
	}

	void GUIRectangleScrollList::updateScale()
	{
		/*Update update element count based on current dimensions. Element size is also updated.*/
		while (size.y < int(updateElementCount) * minElementSize.y && updateElementCount > minVisibleElementCount)
		{//Currently visible elements cannot fit current size
			incrementUpdateElementCount(-1);
			scroll(-1);
		}
		while (size.y >= minElementSize.y * (int(updateElementCount) + 1) && updateElementCount < elements.size())
		{//Else if list size has enough height for another element (min size), increase visible count
			incrementUpdateElementCount(1);
			if (beginElementIndex + updateElementCount == elements.size())
				break;
		}

		//Update element size
		elementSize.x = size.x;
		elementSize.y = int(float(size.y) / float(updateElementCount));
		//Resize width if scroll bar is displayed
		if (elements.size() > updateElementCount && checkState(GUIRECT_OPEN_BIT))
		{//Hidden elements
			elementSize.x -= scrollBar->getWidth();
			scrollUp->setRenderState(getRenderState());
			scrollBar->setRenderState(getRenderState());
			scrollDown->setRenderState(getRenderState());
		}
		else
		{//All elements visible
			scrollUp->setRenderState(false);
			scrollBar->setRenderState(false);
			scrollDown->setRenderState(false);
		}

		GUIRectangle::updateScale();

		//Resizing scroll buttons/bar
		if (elements.size() > updateElementCount)
		{
			if (size.y - updateElementCount * elementSize.y > 0)
				scrollUp->setSize(scrollBar->getWidth(), elementSize.y + 1); //Topmost element has 1 excess y allocated to height
			else
				scrollUp->setSize(scrollBar->getWidth(), elementSize.y);
			scrollBar->setSize(scrollBar->getWidth(), elementSize.y);
			scrollDown->setSize(scrollBar->getWidth(), elementSize.y);
		}

		//Resizing the elements
		if (updateElementCount > 0)
		{
			int excessY = size.y - int(updateElementCount) * elementSize.y;
			//Resize according to element size
			for (size_t i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
			{
				if (excessY-- > 0)
					elements[i]->setSize(elementSize.x, elementSize.y + 1);
				else
					elements[i]->setSize(elementSize);
			}
		}
	}

	void GUIRectangleScrollList::updatePosition()
	{
		GUIRectangle::updatePosition();
		if (updateElementCount <= 0)
			return;

		//Position visible elements
		bool listOnLeftSideOfMainWindow = true;
		if (getXGlobal() + size.x / 2.0f > getBatchManager().window.getWidth() / 2)
			listOnLeftSideOfMainWindow = false;
		int _x = 0;
		if (invisibleElements() && listOnLeftSideOfMainWindow)
			_x = scrollBar->getWidth();
		const size_t lastIndex = beginElementIndex + updateElementCount - 1;
		for (size_t i = lastIndex + 1; i-- > beginElementIndex;)
		{
			if (i == lastIndex)
				elements[i]->setPositionLocal(_x, 0);
			else
				elements[i]->setPositionLocal(_x, elements[i + 1]->getYLocal() + elements[i + 1]->getHeight());
		}

		//Reposition scrolling elements
		if (invisibleElements())
		{//Relative to list position
			int scrollButtonX = 0;
			if (!listOnLeftSideOfMainWindow)
				scrollButtonX = size.x - scrollBar->getWidth();
			scrollDown->setPositionLocal(scrollButtonX, 0);
			scrollUp->setPositionLocal(scrollButtonX, elements[beginElementIndex]->getPositionLocal().y);
			const int barSpace = scrollUp->getYLocal() - scrollDown->getHeight() - elementSize.y;
			const float scrollPercentage = float(beginElementIndex) / float(elements.size() - updateElementCount);
			scrollBar->setPositionLocal(scrollButtonX, elements[beginElementIndex]->getYLocal() - elementSize.y - int(scrollPercentage * barSpace));
		}
	}

	void GUIRectangleScrollList::incrementUpdateElementCount(int incrementation)
	{
		if (incrementation == 0)
			return;

		while (incrementation != 0)
		{
			if (incrementation < 0)
			{//Decrease
				if (updateElementCount <= minVisibleElementCount && elements.size() >= updateElementCount)
					break;//Cannot decrease, no elements are being updated!
				updateElementCount--;
				elements[beginElementIndex + updateElementCount]->setRenderState(false);
				++incrementation;
			}
			else
			{//Increase
				if (beginElementIndex + updateElementCount == elements.size())
				{//Try incrementing from the beginning (cannot increment the end)
					if (beginElementIndex > 0)
					{
						--beginElementIndex;
						elements[beginElementIndex]->setRenderState(getRenderState() && checkState(GUIRECT_OPEN_BIT));
					}
					else
						break;//Cannot increase
				}
				else
				{//Try incrementing from the end
					if (beginElementIndex + updateElementCount < elements.size())
					{
						elements[beginElementIndex + updateElementCount]->setRenderState(getRenderState() && checkState(GUIRECT_OPEN_BIT));
						updateElementCount++;
					}
					else
						break;//Cannot increase
				}
				--incrementation;
			}
		}

		elementSize.y = int(float(size.y) / float(updateElementCount));
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
	}

	void GUIRectangleScrollList::setMinVisibleElementCount(const size_t count)
	{
		minVisibleElementCount = count;
		if (minVisibleElementCount < 4)
			minVisibleElementCount = 4;
		if (updateElementCount < minVisibleElementCount && updateElementCount < elements.size())
			incrementUpdateElementCount(int(std::min(elements.size() - updateElementCount, minVisibleElementCount - updateElementCount)));
		
		disableState(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
	}

	GUIRectangle* GUIRectangleScrollList::addElement(GUIRectangle* element)
	{
		GUIRectangleContainer::addElement(element);
		if (updateElementCount < minVisibleElementCount)
			incrementUpdateElementCount(1);
		elements.back()->setRenderState(getRenderState() && checkState(GUIRECT_OPEN_BIT) && (beginElementIndex + updateElementCount == elements.size()/*is updated*/));
		return element;
	}

	bool GUIRectangleScrollList::moveElement(GUIRectangle* element, const size_t targetIndexBeforeMove)
	{
		if (GUIRectangleContainer::moveElement(element, targetIndexBeforeMove))
		{
			const bool baseRenderState = getRenderState() && checkState(GUIRECT_OPEN_BIT);
			const size_t endElementIndex = beginElementIndex + updateElementCount;
			for (size_t i = 0; i < elements.size(); i++)
			{
				elements[i]->setRenderState(baseRenderState && (i >= beginElementIndex && i < endElementIndex));
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	void GUIRectangleScrollList::setDepth(const int16_t depth)
	{
		GUIRectangleContainer::setDepth(depth);
		scrollUp->setDepth(depth + 1);
		scrollDown->setDepth(depth + 1);
		scrollBar->setDepth(depth + 1);
	}

	void GUIRectangleScrollList::scroll(int amount)
	{
		if (amount > 0)
		{//Scroll down
			for (int i = 0; i < amount; i++)
			{
				if (beginElementIndex + updateElementCount >= elements.size())
					break;//Do not scroll any more downwards
				elements[beginElementIndex + updateElementCount]->setRenderState(true);//Make the bottom-most element visible
				elements[beginElementIndex]->setRenderState(false);//Make the top-most element invisible
				beginElementIndex++;
			}
		}
		else if (amount < 0)
		{//Scroll up
			amount *= -1;//flip amount for for loop
			for (int i = 0; i < amount; i++)
			{
				if (beginElementIndex <= 0)
					break;//Do not scroll any more upwards
				beginElementIndex--;
				elements[beginElementIndex + updateElementCount]->setRenderState(false);//Make the bottom-most element invisible
				elements[beginElementIndex]->setRenderState(true);//Make the top-most element visible
			}
		}
		else
			return;

		//Reposition in the next post update
		disableBit(state, GUIRECT_SCALE_UPDATED_BIT);
		disableBit(state, GUIRECT_POSITION_UPDATED_BIT);
	}
}