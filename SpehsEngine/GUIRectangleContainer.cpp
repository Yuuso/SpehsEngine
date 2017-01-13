#include "GUIRectangleContainer.h"
#include "GUIRectangleTree.h"
#include "GUIStringEditor.h"
#include "Polygon.h"


namespace spehs
{
	GUIRectangleContainer::GUIRectangleContainer()
	{
		open();
	}
	GUIRectangleContainer::~GUIRectangleContainer()
	{
		for (unsigned i = 0; i < elements.size(); i++)
			delete elements[i];
	}
	void GUIRectangleContainer::inputUpdate()
	{
		disableState(GUIRECT_MOUSE_HOVER_CONTAINER);

		//Updating elements
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (unsigned i = 0; i < elements.size(); i++)
			{
				elements[i]->inputUpdate();
				//If element is under mouse, mark this container as under mouse
				if (elements[i]->getMouseHoverAny())
					enableState(GUIRECT_MOUSE_HOVER_CONTAINER);
			}
		}

		//Update this
		GUIRectangle::inputUpdate();

		//If mouse is hovering over the base rectangle, enable container hovering bit
		if (checkState(GUIRECT_MOUSE_HOVER))
			enableState(GUIRECT_MOUSE_HOVER_CONTAINER);

	}
	void GUIRectangleContainer::visualUpdate()
	{
		//Call input update on self
		GUIRectangle::visualUpdate();

		//Call visual update for all the elements AFTER my input
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->visualUpdate();
		}
	}
	void GUIRectangleContainer::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(_state);
		}
		else
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setRenderState(false);
		}
	}
	bool GUIRectangleContainer::isReceivingInput() const
	{
		if (!checkState(GUIRECT_OPEN_BIT) || !getInputEnabled())
			return false;
		if (checkState(GUIRECT_RECEIVING_INPUT))
			return true;

		//Check activity in elements
		if (checkState(GUIRECT_OPEN_BIT))
		{
			for (unsigned i = 0; i < elements.size(); i++)
			{
				if (elements[i]->isReceivingInput())
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
		if (parent)
			parent->disableStateRecursive(GUIRECT_MIN_SIZE_UPDATED_BIT);
	}
	void GUIRectangleContainer::addElement(GUIRectangle* element)
	{
		elements.push_back(element);
		elements.back()->setParent(this);
		elements.back()->setDepth(getDepth() + 10);

		//Render state
		element->setRenderState(getRenderState() && checkState(GUIRECT_OPEN_BIT));

		//Enabled state
		element->setInputEnabled(getInputEnabled());

		//Min size must be updated for everything above
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT | GUIRECT_SCALE_UPDATED_BIT | GUIRECT_POSITION_UPDATED_BIT);
	}
	bool GUIRectangleContainer::removeElement(GUIRectangle* element)
	{
		for (unsigned i = 0; i < elements.size(); i++)
		{
			if (elements[i] == element)
			{
				delete elements[i];
				elements.erase(elements.begin() + i);
				disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
				disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
				disableStateRecursiveUpwards(GUIRECT_POSITION_UPDATED_BIT);
				return true;
			}
		}
		return false;
	}
	void GUIRectangleContainer::enableStateRecursive(const GUIRECT_STATE_TYPE stateBit)
	{
		GUIRectangle::enableState(stateBit);
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->enableStateRecursive(stateBit);
	}
	void GUIRectangleContainer::disableStateRecursive(const GUIRECT_STATE_TYPE stateBit)
	{
		GUIRectangle::disableState(stateBit);
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->disableStateRecursive(stateBit);
	}
	void GUIRectangleContainer::onEnableInput()
	{
		GUIRectangle::onEnableInput();
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->enableInput();
	}
	void GUIRectangleContainer::onDisableInput()
	{
		GUIRectangle::onDisableInput();
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->disableInput();
	}
	bool GUIRectangleContainer::open()
	{//Open container dimension
		if (checkState(GUIRECT_OPEN_BIT))
			return false;

		enableState(GUIRECT_OPEN_BIT);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);
		setRenderState(true);
		return true;
	}
	bool GUIRectangleContainer::close()
	{//Close container dimension
		if (!checkState(GUIRECT_OPEN_BIT))
			return false;

		disableState(GUIRECT_OPEN_BIT);
		disableStateRecursiveUpwards(GUIRECT_MIN_SIZE_UPDATED_BIT);//Make all above update size
		disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT);//Make all above update scale and therefore position
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->setRenderState(false);
		return true;
	}
	void GUIRectangleContainer::toggleOpen()
	{
		if (checkState(GUIRECT_OPEN_BIT))
			close();
		else
			open();
	}
	void GUIRectangleContainer::setDepth(const int16_t depth)
	{
		GUIRectangle::setDepth(depth);
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->setDepth(depth + 10);
	}
	bool GUIRectangleContainer::isDescendant(GUIRectangle* element) const
	{
		for (unsigned i = 0; i < elements.size(); i++)
		{
			if (elements[i] == element)
				return true;
			if (elements[i]->getAsGUIRectangleContainerPtr())
			{
				if (elements[i]->getAsGUIRectangleContainerPtr()->isDescendant(element))
					return true;
			}
		}
		return false;
	}
}