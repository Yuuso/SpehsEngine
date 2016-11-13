#include "GUIRectangleContainer.h"
#include "GUIRectangleTree.h"
#include "GUITextField.h"
#include "Polygon.h"


namespace spehs
{
	GUIRectangleContainer::GUIRectangleContainer() : beginElementIndex(0), updateElementCount(0)
	{
		open();
	}
	GUIRectangleContainer::~GUIRectangleContainer()
	{
		for (unsigned i = 0; i < elements.size(); i++)
			delete elements[i];
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
	void GUIRectangleContainer::postUpdate()
	{
		GUIRectangle::postUpdate();
		if (checkState(GUIRECT_OPEN))
		{
			for (int i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
				elements[i]->postUpdate();
		}
	}
	void GUIRectangleContainer::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		if (checkState(GUIRECT_OPEN))
		{
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (i < beginElementIndex || i > getEndElementIndex())
					elements[i]->setRenderState(false);
				else
					elements[i]->setRenderState(_state);
			}
		}
		else
		{//Container is not open
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(false);
		}
	}
	bool GUIRectangleContainer::isReceivingInput()
	{
		if (!isOpen())
			return false;
		if (checkState(GUIRECT_RECEIVING_INPUT))
			return true;

		//Check activity in elements
		for (unsigned i = 0; i < updateElementCount; i++)
		{
			if (elements[beginElementIndex + i]->isReceivingInput())
				return true;
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
		elements.back()->setDepth(getDepth() + 10);

		//Render state
		if (checkState(GUIRECT_OPEN))
			element->setRenderState(getRenderState());
		else
			element->setRenderState(false);

		//Enable state
		if (checkState(GUIRECT_ENABLED_BIT))
			element->enableStateRecursive(GUIRECT_ENABLED_BIT);
		else
			element->disableStateRecursive(GUIRECT_ENABLED_BIT);

		//Min size must be updated for everything above
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED);
		
		//Element update tracking
		incrementUpdateElementCount(1);
	}
	bool GUIRectangleContainer::removeElement(GUIRectangle* element)
	{
		for (unsigned i = 0; i < elements.size(); i++)
		{
			if (elements[i] == element)
			{
				delete elements[i];
				elements.erase(elements.begin() + i);
				disableBit(state, GUIRECT_SCALED);
				disableBit(state, GUIRECT_POSITIONED);
				if (updateElementCount > elements.size())
					--updateElementCount;
				return true;
			}
		}
		return false;
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
	void GUIRectangleContainer::onEnable()
	{
		GUIRectangle::onEnable();
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->onEnable();
	}
	void GUIRectangleContainer::onDisable()
	{
		GUIRectangle::onDisable();
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->onDisable();
	}
	void GUIRectangleContainer::incrementUpdateElementCount(int incrementation)
	{
		do
		{
			if (incrementation < 0)
			{//Try to decrease
				if (updateElementCount == 0)
					break;
				elements[getEndElementIndex()]->setRenderState(false);
				updateElementCount--;
				++incrementation;
			}
			else
			{//Try to increase
				if (getEndElementIndex() + 1 == elements.size())
					break;
				updateElementCount++;
				if (polygon->getRenderState())
					elements[getEndElementIndex()]->setRenderState(true);
				else
					elements[getEndElementIndex()]->setRenderState(false);
				--incrementation;
			}
		} while (incrementation != 0);

		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_POSITIONED);
	}
	void GUIRectangleContainer::scroll(int amount)
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
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_POSITIONED);
		
	}
	void GUIRectangleContainer::open()
	{//Open container dimension
		enableBit(state, GUIRECT_OPEN);
		disableBit(state, GUIRECT_POSITIONED);
		disableBit(state, GUIRECT_SCALED);
		disableBit(state, GUIRECT_MIN_SIZE_UPDATED);
		setRenderState(true);
	}
	void GUIRectangleContainer::close()
	{//Close container dimension
		disableBit(state, GUIRECT_OPEN);
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->setRenderState(false);
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
	void GUIRectangleContainer::setDepth(uint16_t depth)
	{
		GUIRectangle::setDepth(depth);
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->setDepth(depth + 10);
	}
}