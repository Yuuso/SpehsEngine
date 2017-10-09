#include <algorithm>
#include "InputManager.h"
#include "ApplicationData.h"
#include "GUIRectangleScrollList.h"
#include "GUIWindow.h"
#include "GUIStringEditor.h"
#include "GUIRectangle.h"


namespace spehs
{
	namespace ScrollButtons{ enum ScrollButtons{ up, bar, down }; }
	int GUIRectangleScrollList::defaultScrollBarWidth = 20;
	spehs::Color GUIRectangleScrollList::defaultScrollElementColor(1.0f, 1.0f, 1.0f, 1.0f);
	std::string GUIRectangleScrollList::scrollUpTexturePath;
	std::string GUIRectangleScrollList::scrollBarTexturePath;
	std::string GUIRectangleScrollList::scrollDownTexturePath;
	GUIRectangleScrollList::GUIRectangleScrollList() : beginElementIndex(0), updateElementCount(0), minVisibleElementCount(4)
	{
		disableState(GUIRECT_HOVER_COLOR_BIT);
		scrollUp = new GUIRectangle(ScrollButtons::up);
		scrollBar = new GUIRectangle(ScrollButtons::bar);
		scrollDown = new GUIRectangle(ScrollButtons::down);
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
	GUIRectangleScrollList::GUIRectangleScrollList(const GUIRECT_ID_TYPE id) : GUIRectangleScrollList()
	{
		setID(id);
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
		beginElementIndex = 0;
		updateElementCount = 0;
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
		for (unsigned i = 0; i < elements.size(); i++)
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
		return minElementSize.y * elements.size();
	}
	void GUIRectangleScrollList::inputUpdate(InputUpdateData& data)
	{
		if (getInputEnabled() && checkState(GUIRECT_OPEN_BIT))
		{
			scrollUp->inputUpdate(data);
			scrollBar->inputUpdate(data);
			scrollDown->inputUpdate(data);

			if (inputManager->isKeyDown(MOUSE_BUTTON_LEFT) && //Mouse left is held
				inputManager->getMouseY() > scrollDown->getYGlobal() + scrollDown->getHeight() && //mouse is within scroll bar area of movement
				inputManager->getMouseY() < scrollUp->getYGlobal() &&
				inputManager->getMouseX() > scrollDown->getXGlobal() &&
				inputManager->getMouseX() < scrollDown->getXGlobal() + scrollDown->getWidth())
			{//Mouse dragging scroll bar

				//Calculate scroll based on mouse position relative to scroll area
				float scrollAreaBegin = scrollDown->getYGlobal() + scrollDown->getHeight() + scrollBar->getHeight() / 2.0f;//Y value of the scrolling area beginning
				float scrollAreaHeight = scrollUp->getYGlobal() - scrollDown->getYGlobal() - scrollDown->getHeight() - scrollBar->getHeight();//Height of the scrolling area in between min and max positions of scroll bar center
				scroll(round((elements.size() - updateElementCount) * (1.0f - (inputManager->getMouseY() - scrollAreaBegin) / scrollAreaHeight)) - beginElementIndex);
			}

			if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) && invisibleElements())
			{
				if (scrollUp->getMouseHover())
					scroll(-1);
				else if (scrollDown->getMouseHover())
					scroll(1);
			}
			if (getMouseHover() && inputManager->getMouseWheelDelta())
				scroll(-inputManager->getMouseWheelDelta());
		}

		disableState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT);
		GUIRectangle::inputUpdate(data);
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (unsigned i = 0; i < updateElementCount; i++)
			{
				elements[beginElementIndex + i]->inputUpdate(data);
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
			for (unsigned i = 0; i < elements.size(); i++)
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
			for (unsigned i = 0; i < elements.size(); i++)
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
			minSize.y = minElementSize.y * std::min(minVisibleElementCount, updateElementCount);

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
		while (size.y < updateElementCount * minElementSize.y && updateElementCount > minVisibleElementCount)
		{//Currently visible elements cannot fit current size
			incrementUpdateElementCount(-1);
			scroll(-1);
		}
		while (size.y >= minElementSize.y * (updateElementCount + 1) && updateElementCount < elements.size())
		{//Else if list size has enough height for another element (min size), increase visible count
			incrementUpdateElementCount(1);
			if (beginElementIndex + updateElementCount == elements.size())
				break;
		}

		//Update element size
		elementSize.x = size.x;
		elementSize.y = size.y / float(updateElementCount);
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
			int excessY = size.y - updateElementCount * elementSize.y;
			//Resize according to element size
			for (int i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
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
		if (getXGlobal() + size.x / 2.0f > spehs::ApplicationData::getWindowWidthHalf())
			listOnLeftSideOfMainWindow = false;
		int _x = 0;
		if (invisibleElements() && listOnLeftSideOfMainWindow)
			_x = scrollBar->getWidth();
		for (int i = beginElementIndex + updateElementCount - 1/*last*/; i >= beginElementIndex; i--)
		{
			if (i == beginElementIndex + updateElementCount - 1/*last*/)
				elements[i]->setPositionLocal(_x, 0);
			else
				elements[i]->setPositionLocal(_x, elements[i + 1]->getYLocal() + elements[i + 1]->getHeight());
		}

		//Reposition scrolling elements
		if (invisibleElements())
		{//Relative to list position
			float scrollButtonX = 0;
			if (!listOnLeftSideOfMainWindow)
				scrollButtonX = size.x - scrollBar->getWidth();
			scrollDown->setPositionLocal(scrollButtonX, 0);
			scrollUp->setPositionLocal(scrollButtonX, elements[beginElementIndex]->getPositionLocal().y);
			float barSpace = scrollUp->getYLocal() - scrollDown->getHeight() - elementSize.y;
			float scrollPercentage = beginElementIndex / float(int(elements.size()) - updateElementCount);
			scrollBar->setPositionLocal(scrollButtonX, elements[beginElementIndex]->getYLocal() - elementSize.y - scrollPercentage * barSpace);
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

		elementSize.y = size.y / float(updateElementCount);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIRectangleScrollList::setMinVisibleElementCount(const int count)
	{
		minVisibleElementCount = count;
		if (minVisibleElementCount < 4)
			minVisibleElementCount = 4;
		if (updateElementCount < minVisibleElementCount && updateElementCount < elements.size())
			incrementUpdateElementCount(std::min((int)elements.size() - updateElementCount, minVisibleElementCount - updateElementCount));
		
		disableState(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
	}
	void GUIRectangleScrollList::addElement(GUIRectangle* element)
	{
		GUIRectangleContainer::addElement(element);
		if (updateElementCount < minVisibleElementCount)
			incrementUpdateElementCount(1);
		elements.back()->setRenderState(getRenderState() && checkState(GUIRECT_OPEN_BIT) && (beginElementIndex + updateElementCount == elements.size()/*is updated*/));
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
			for (unsigned i = 0; i < amount; i++)
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
			for (unsigned i = 0; i < amount; i++)
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