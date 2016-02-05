#include "GUIRectangleContainer.h"
#include "GUIRectangleTree.h"
#include "GUITextField.h"


namespace SpehsEngine
{
	GUIRectangleContainer::GUIRectangleContainer() : beginElementIndex(0), updateElementCount(0)
	{
		enableBit(state, GUIRECT_OPEN);
	}
	GUIRectangleContainer::~GUIRectangleContainer()
	{
		clear();
	}
	void GUIRectangleContainer::update()
	{
		GUIRectangle::update();

		//If mouse is hovering over the base rectangle, enable container hovering bit
		if (checkBit(state, GUIRECT_MOUSE_HOVER))
			enableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
		else
			disableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);

		if (isOpen())
		{//Update elements
			for (int i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
			{
				elements[i]->update();
				//If element is under mouse, mark this container as under mouse
				if (elements[i]->getMouseHoverAny())
					enableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
			}
		}
	}

	void GUIRectangleContainer::render()
	{
		if (!checkBit(state, GUIRECT_VISIBLE))
			return;

		GUIRectangle::render();

		if (isOpen())
		{//Render elements
			for (int i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
				elements[i]->render();
		}
	}
	void GUIRectangleContainer::postRender()
	{
		GUIRectangle::postRender();
		
		if (isOpen())
		{//Post render elements
			for (int i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
				elements[i]->postRender();
		}
	}
	bool GUIRectangleContainer::isReceivingTextInput()
	{
		if (isOpen())
		{//Check activity in elements
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (elements[i]->getAsGUITextFieldPtr() && elements[i]->getAsGUITextFieldPtr()->isReceivingInput())
					return true;
				if (elements[i]->getAsGUIRectangleContainerPtr() && elements[i]->getAsGUIRectangleContainerPtr()->isReceivingTextInput())
					return true;
			}
		}
		return false;
	}
	void GUIRectangleContainer::clear()
	{
		for (unsigned i = 0; i < elements.size(); i++)
			delete elements[i];
		elements.clear();
		beginElementIndex = 0;
		updateElementCount = 0;
		minElementSize.x = 0;
		minElementSize.y = 0;
		if (parent)
			parent->disableState(GUIRECT_SCALED);
	}
	void GUIRectangleContainer::addElement(GUIRectangle* element)
	{
		elements.push_back(element);
		elements.back()->setParent(this);

		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED);
		
		//Element update tracking
		incrementUpdateElementCount(1);
	}
	void GUIRectangleContainer::enableStateRecursive(GUIRECT_STATE_TYPE stateBit)
	{
		GUIRectangle::enableState(stateBit);
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->enableStateRecursive(stateBit);
	}
	void GUIRectangleContainer::disableStateRecursive(GUIRECT_STATE_TYPE stateBit)
	{
		GUIRectangle::disableState(stateBit);
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->disableStateRecursive(stateBit);
	}
	int GUIRectangleContainer::getEndElementIndex()
	{
		if (updateElementCount <= 0)
			return -1;
		return beginElementIndex + updateElementCount - 1;
	}
	void GUIRectangleContainer::loseFocus()
	{
		GUIRectangle::loseFocus();
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->loseFocus();
	}
	void GUIRectangleContainer::incrementUpdateElementCount(int incrementation)
	{
		updateElementCount += incrementation;
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_POSITIONED);
	}
	void GUIRectangleContainer::scroll(int amount)
	{
		//Increment begin index value
		beginElementIndex += amount;

		////Limit begin index
		//Scrolled below the bottom
		if (beginElementIndex + updateElementCount > elements.size())
			beginElementIndex = int(elements.size()) - updateElementCount;
		//Scrolled over the top
		if (beginElementIndex < 0)
			beginElementIndex = 0;

		//Reposition in the next rendering phase
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_POSITIONED);
	}
	void GUIRectangleContainer::open()
	{//Open container dimension
		enableBit(state, GUIRECT_OPEN);
		disableBit(state, GUIRECT_POSITIONED);
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_MIN_SIZE_UPDATED);
		//for (unsigned i = 0; i < elements.size(); i++)
		//{
		//	elements[i]->disableState(GUIRECT_SCALED);
		//	elements[i]->disableState(GUIRECT_POSITIONED);
		//}
	}
	void GUIRectangleContainer::close()
	{//Close container dimension
		disableBit(state, GUIRECT_OPEN);
	}
	void GUIRectangleContainer::closeTreeElements()
	{
		for (int i = beginElementIndex; i <= getEndElementIndex(); i++)
			if (elements[i]->getAsGUIRectangleContainerPtr())
			{
			if (elements[i]->getAsGUIRectangleTreePtr())
				elements[i]->getAsGUIRectangleTreePtr()->close();
			else
				elements[i]->getAsGUIRectangleContainerPtr()->closeTreeElements();
			}
	}
	void GUIRectangleContainer::closeTreeElements(GUIRectangle* excluded)
	{
		for (int i = beginElementIndex; i <= getEndElementIndex(); i++)
			if (elements[i]->getAsGUIRectangleContainerPtr())
			{
			if (elements[i]->getAsGUIRectangleTreePtr() && elements[i] != excluded)
				elements[i]->getAsGUIRectangleTreePtr()->close();
			else
				elements[i]->getAsGUIRectangleContainerPtr()->closeTreeElements(excluded);
			}
	}
}