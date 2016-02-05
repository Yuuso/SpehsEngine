#include "InputManager.h"
#include "ApplicationData.h"
#include "GUIRectangleList.h"
#include "GUIWindow.h"
#include "GUITextField.h"
#include "GUIRectangle.h"
#define MIN_VISIBLE_COUNT 4


namespace SpehsEngine
{
	namespace ScrollButtons{ enum ScrollButtons{ up, bar, down }; }
	GUIRectangleList::GUIRectangleList() : GUIRectangleContainer()
	{
		setColor(50, 50, 50);
		disableBit(state, GUIRECT_HOVER_COLOR);

		scrollUp = new GUIRectangle(ScrollButtons::up);
		scrollBar = new GUIRectangle(ScrollButtons::bar);
		scrollDown = new GUIRectangle(ScrollButtons::down);

		scrollUp->setParent(this);
		scrollBar->setParent(this);
		scrollDown->setParent(this);

		scrollUp->setString("U");
		scrollBar->setString("B");
		scrollDown->setString("D");
	}
	GUIRectangleList::GUIRectangleList(GUIRECT_ID_TYPE id) : GUIRectangleList()
	{
		setID(id);
	}
	GUIRectangleList::~GUIRectangleList()
	{
		delete scrollUp;
		delete scrollBar;
		delete scrollDown;
		clear();
	}
	void GUIRectangleList::update()
	{
		scrollUp->update();
		scrollBar->update();
		scrollDown->update();

		if (inputManager->isKeyDown(MOUSE_BUTTON_LEFT) && //Mouse left is held
			inputManager->getMouseY() > scrollDown->getY() + scrollDown->getHeight() && //mouse is within scroll bar area of movement
			inputManager->getMouseY() < scrollUp->getY() &&
			inputManager->getMouseX() > scrollDown->getX() &&
			inputManager->getMouseX() < scrollDown->getX() + scrollDown->getWidth())
		{//Mouse dragging scroll bar

			//Calculate scroll based on mouse position relative to scroll area
			float scrollAreaBegin = scrollDown->getY() + scrollDown->getHeight() + scrollBar->getHeight() / 2.0f;//Y value of the scrolling area beginning
			float scrollAreaHeight = scrollUp->getY() - scrollDown->getY() - scrollDown->getHeight() - scrollBar->getHeight();//Height of the scrolling area in between min and max positions of scroll bar center
			scroll(round((elements.size() - updateElementCount) * (1.0f - (inputManager->getMouseY() - scrollAreaBegin) / scrollAreaHeight)) - beginElementIndex);
		}

		if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT) && invisibleElements())
		{
			if (scrollUp->getMouseHover())
				scroll(-1);
			else if (scrollDown->getMouseHover())
				scroll(1);
		}
		if (isFocused() && inputManager->getMouseWheelDelta())
			scroll(-inputManager->getMouseWheelDelta());

		GUIRectangleContainer::update();
	}
	void GUIRectangleList::render()
	{
		GUIRectangleContainer::render();

		if (invisibleElements())
		{
			scrollUp->render();
			scrollBar->render();
			scrollDown->render();
		}
	}
	void GUIRectangleList::postRender()
	{
		GUIRectangleContainer::postRender();

		if (invisibleElements())
		{
			scrollUp->postRender();
			scrollBar->postRender();
			scrollDown->postRender();
		}
	}
	void GUIRectangleList::updatePosition()
	{

		//Update element size in case changes have been made
		//updateMinSize();

		GUIRectangle::updatePosition();
		if (updateElementCount <= 0)
			return;

		//Position visible elements
		bool listOnLeftSideOfMainWindow = true;
		if (getX() + size.x / 2.0f > applicationData->getWindowWidthHalf())
			listOnLeftSideOfMainWindow = false;
		int _x = 0;
		if (invisibleElements() && listOnLeftSideOfMainWindow)
			_x = SCROLL_BUTTON_WIDTH;
		for (int i = getEndElementIndex(); i >= beginElementIndex; i--)
		{
			if (i == getEndElementIndex())
				elements[i]->setPosition(_x, 0);
			else
				elements[i]->setPosition(_x, elements[i + 1]->getLocalY() + elements[i + 1]->getHeight());
		}

		//Reposition scrolling elements
		if (invisibleElements())
		{//Relative to list position
			float scrollButtonX = 0;
			if (!listOnLeftSideOfMainWindow)
				scrollButtonX = size.x - SCROLL_BUTTON_WIDTH;
			scrollDown->setPosition(scrollButtonX, 0);
			scrollUp->setPosition(scrollButtonX, elements[beginElementIndex]->getLocalPosition().y);
			float barSpace = scrollUp->getLocalY() - scrollDown->getHeight() - elementSize.y;
			float scrollPercentage = beginElementIndex / float(int(elements.size()) - updateElementCount);
			scrollBar->setPosition(scrollButtonX, elements[beginElementIndex]->getLocalY() - elementSize.y - scrollPercentage * barSpace);
		}
	}
	void GUIRectangleList::updateScale()
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
			for (int i = beginElementIndex; i <= getEndElementIndex(); i++)
			{
				if (excessY-- > 0)
					elements[i]->setSize(elementSize.x, elementSize.y + 1);
				else
					elements[i]->setSize(elementSize);
			}
		}

		disableBit(state, GUIRECT_POSITIONED);
	}
	void GUIRectangleList::incrementUpdateElementCount(int incrementation)
	{
		GUIRectangleContainer::incrementUpdateElementCount(incrementation);
		elementSize.y = size.y / float(updateElementCount);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED);
	}
	void GUIRectangleList::updateUpdateElementCount()
	{
		//If reached min size, reduce visible element count by 1
		if (minSize.y == size.y && updateElementCount > MIN_VISIBLE_COUNT)
		{
			incrementUpdateElementCount(-1);
			beginElementIndex = 0;
		}
		while (size.y > minElementSize.y * (updateElementCount + 1) && updateElementCount < elements.size())
		{//Else if list size has enough height for another element (min size), increase visible count
			incrementUpdateElementCount(1);
			beginElementIndex = 0;
		}

		//Update element size
		elementSize.x = size.x;
		elementSize.y = size.y / float(updateElementCount);
		//Resize width if scroll bar is displayed
		if (elements.size() > updateElementCount)
			elementSize.x -= SCROLL_BUTTON_WIDTH;
	}
	void GUIRectangleList::addElement(GUIRectangle* e)
	{
		GUIRectangleContainer::addElement(e);
		if (updateElementCount > MIN_VISIBLE_COUNT)
			incrementUpdateElementCount(-1);
		if (beginElementIndex == -1)
			beginElementIndex = 0;
	}
	void GUIRectangleList::updateMinSize()
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
		minSize.x = minElementSize.x;
		minSize.y = updateElementCount * minElementSize.y;

		//Account scroll button width into min width
		if (invisibleElements())
			minSize.x += SCROLL_BUTTON_WIDTH;
	}
}