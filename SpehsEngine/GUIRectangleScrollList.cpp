#include "InputManager.h"
#include "ApplicationData.h"
#include "GUIRectangleScrollList.h"
#include "GUIWindow.h"
#include "GUIStringEditor.h"
#include "GUIRectangle.h"
#define MIN_VISIBLE_COUNT 4


namespace spehs
{
	namespace ScrollButtons{ enum ScrollButtons{ up, bar, down }; }
	GUIRectangleScrollList::GUIRectangleScrollList() : beginElementIndex(0), updateElementCount(0)
	{
		setColor(50, 50, 50);
		disableState(GUIRECT_HOVER_COLOR);

		scrollUp = new GUIRectangle(ScrollButtons::up);
		scrollBar = new GUIRectangle(ScrollButtons::bar);
		scrollDown = new GUIRectangle(ScrollButtons::down);
		scrollUp->setParent(this);
		scrollBar->setParent(this);
		scrollDown->setParent(this);
		scrollUp->setString("U");
		scrollBar->setString("B");
		scrollDown->setString("D");
		scrollUp->setRenderState(getRenderState());
		scrollBar->setRenderState(getRenderState());
		scrollDown->setRenderState(getRenderState());
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
	void GUIRectangleScrollList::inputUpdate()
	{
		if (getInputEnabled())
		{
			scrollUp->inputUpdate();
			scrollBar->inputUpdate();
			scrollDown->inputUpdate();

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

		disableState(GUIRECT_MOUSE_HOVER_CONTAINER);
		GUIRectangle::inputUpdate();
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (unsigned i = 0; i < updateElementCount; i++)
			{
				elements[beginElementIndex + i]->inputUpdate();
				if (elements[beginElementIndex + i]->getMouseHoverAny())
					enableState(GUIRECT_MOUSE_HOVER_CONTAINER);
			}
		}
	}
	void GUIRectangleScrollList::visualUpdate()
	{
		GUIRectangleContainer::visualUpdate();
		scrollUp->visualUpdate();
		scrollBar->visualUpdate();
		scrollDown->visualUpdate();
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
		GUIRectangleUnisizeContainer::updateMinSize();

		minSize.x = minElementSize.x;
		minSize.y = minElementSize.y * updateElementCount;

		//Account scroll button width into min width
		if (invisibleElements())
			minSize.x += SCROLL_BUTTON_WIDTH;
	}
	void GUIRectangleScrollList::updateScale()
	{
		updateUpdateElementCount();
		GUIRectangle::updateScale();

		//Resizing scroll buttons/bar
		if (elements.size() > updateElementCount)
		{
			if (size.y - updateElementCount * elementSize.y > 0)
				scrollUp->setSize(SCROLL_BUTTON_WIDTH, elementSize.y + 1); //Topmost element has 1 excess y allocated to height
			else
				scrollUp->setSize(SCROLL_BUTTON_WIDTH, elementSize.y);
			scrollBar->setSize(SCROLL_BUTTON_WIDTH, elementSize.y);
			scrollDown->setSize(SCROLL_BUTTON_WIDTH, elementSize.y);
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
		if (getXGlobal() + size.x / 2.0f > applicationData->getWindowWidthHalf())
			listOnLeftSideOfMainWindow = false;
		int _x = 0;
		if (invisibleElements() && listOnLeftSideOfMainWindow)
			_x = SCROLL_BUTTON_WIDTH;
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
				scrollButtonX = size.x - SCROLL_BUTTON_WIDTH;
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
				if (updateElementCount <= MIN_VISIBLE_COUNT && elements.size() >= updateElementCount)
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
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIRectangleScrollList::updateUpdateElementCount()
	{
		//If reached min size, reduce visible element count by 1
		if (minSize.y == size.y && updateElementCount > MIN_VISIBLE_COUNT)
		{
			incrementUpdateElementCount(-1);
			scroll(-1);
		}
		while (size.y > minElementSize.y * (updateElementCount + 1) && updateElementCount < elements.size())
		{//Else if list size has enough height for another element (min size), increase visible count
			incrementUpdateElementCount(1);
			if (beginElementIndex + updateElementCount == elements.size())
				break;
		}

		//Update element size
		elementSize.x = size.x;
		elementSize.y = size.y / float(updateElementCount);
		//Resize width if scroll bar is displayed
		if (elements.size() > updateElementCount)
		{//Hidden elements
			elementSize.x -= SCROLL_BUTTON_WIDTH;
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
	}
	void GUIRectangleScrollList::addElement(GUIRectangle* element)
	{
		GUIRectangleContainer::addElement(element);
		if (updateElementCount < MIN_VISIBLE_COUNT)
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