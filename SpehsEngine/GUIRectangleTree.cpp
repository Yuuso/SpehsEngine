#include <iostream>
#include "Polygon.h"
#include "InputManager.h"
#include "ApplicationData.h"
#include "GUIRectangleTree.h"
#include "GUIRectangle.h"
#include "Time.h"
#define TREE_OPEN_TIME 500

namespace spehs
{
	GUIRectangleTree::GUIRectangleTree() : pressedLeafNodeID(0), openTreeButton(MOUSE_BUTTON_LEFT)
	{
		close();
	}
	GUIRectangleTree::GUIRectangleTree(int _ID) : GUIRectangleTree()
	{
		setID(_ID);
	}
	GUIRectangleTree::GUIRectangleTree(std::string str) : GUIRectangleTree()
	{
		setString(str);
	}
	GUIRectangleTree::~GUIRectangleTree()
	{
	}
	void GUIRectangleTree::update()
	{//NOTE: do not use container update, tree requires special function call ordering
		
		GUIRectangle::update();
		//If mouse is hovering over the base rectangle, enable container hovering bit
		if (checkBit(state, GUIRECT_MOUSE_HOVER))
			enableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
		else
			disableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);

		////Closing / Opening
		if (pressedLeafNodeID)
		{//Leaf node was pressed, close tree
			close();
		}
		else
		{//Button press handling ->open tree or inform root of leaf press
			if (openTreeButton)
			{//Toggle open when open button is pressed while mouse hovering
				if (getMouseHover() && inputManager->isKeyPressed(openTreeButton))
				{//If button is a tree and was pressed...
					if (elements.size() == 0)//If this is button tree, must not have nodes underneath to trigger leaf node press
						leafNodePressed(id);
					else//Contains nodes, toggle open
						toggleOpen();
				}
			}
			else
			{//Open when mouse is hovering over
				if (checkState(GUIRECT_MOUSE_HOVER_CONTAINER) && checkState(GUIRECT_ENABLED_BIT))
					open();
				else if (treeOpenTimer <= 0)
					close();
			}
		}


		pressedLeafNodeID = 0;//Reset pressed leaf node id before next node update

		if (checkState(GUIRECT_OPEN))
		{//Update elements
			for (int i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
			{
				elements[i]->update();
				//If element is under mouse, mark this container as under mouse
				if (elements[i]->getMouseHoverAny())
					enableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);
			}
		}
		if (checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER))
		{//Mouse hovering over the container
			treeOpenTimer = TREE_OPEN_TIME;
		}
		else if (treeOpenTimer > 0)
		{//If mouse is outside the container, close over time
			treeOpenTimer -= time::getDeltaTimeAsMilliseconds();
			if (treeOpenTimer <= 0)
				close();
			else
				enableBit(state, GUIRECT_MOUSE_HOVER_CONTAINER);//Keep container hover active for the duration of open timer
		}
	}
	void GUIRectangleTree::setRenderState(const bool _state)
	{
		GUIRectangle::setRenderState(_state);
		if (!checkState(GUIRECT_OPEN))
		{
			for (int i = beginElementIndex; i <= getEndElementIndex(); i++)
				elements[i]->setRenderState(false);
		}
		else
		{
			for (int i = beginElementIndex; i <= getEndElementIndex(); i++)
				elements[i]->setRenderState(_state);
		}
	}
	void GUIRectangleTree::updateScale()
	{
		GUIRectangle::updateScale();

		if (isOpen())
		{
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setSize(minElementSize.x, minElementSize.y);
		}
	}
	void GUIRectangleTree::updatePosition()
	{
		GUIRectangle::updatePosition();
		
		if (isOpen())
		{
			////Element X positioning
			if (parent->getAsGUIRectangleTreePtr())
			{//Parent tree exists, mimic parent branch direction
				if (parent->getAsGUIRectangleTreePtr()->branchX > 0)
					branchX = size.x;//Parent branches right
				else
					branchX = -minElementSize.x;//Parent branches left
			}
			else
			{//Parent TREE doesn't exist. Decide branching direction based on horizontal location/size
				GUIRectangle* firstGenParent = getFirstGenerationParent();
				if (firstGenParent->getXGlobal() + firstGenParent->getWidth() / 2.0f > applicationData->getWindowWidthHalf())
				{//Branch left
					branchX = firstGenParent->getXGlobal() - getXGlobal() - minElementSize.x;
				}
				else
				{//Branch right
					branchX = firstGenParent->getXGlobal() - getXGlobal() + firstGenParent->getWidth();
				}
			}

			////Element Y positioning
			int branchY(getYGlobal());
			if (branchY + updateElementCount * minElementSize.y + 1 > applicationData->getWindowHeight())
				branchY = applicationData->getWindowHeight() - updateElementCount * minElementSize.y - 1;

			//Position elements
			for (int i = beginElementIndex; i <= getEndElementIndex(); i++)
				elements[i]->setPositionGlobal(getXGlobal() + branchX, branchY + i * minElementSize.y);
		}
	}
	void GUIRectangleTree::leafNodePressed(int _ID)
	{
		if (parent->getAsGUIRectangleTreePtr())
		{
			parent->getAsGUIRectangleTreePtr()->leafNodePressed(_ID);
			return;
		}
		pressedLeafNodeID = _ID;
	}
	void GUIRectangleTree::addElement(GUIRectangle* element)
	{
		GUIRectangleContainer::addElement(element);
		element->setRenderState(checkState(GUIRECT_OPEN));

		//By default, make tree to open on mouse hover when node element is added
		openTreeButton = 0;
	}
	void GUIRectangleTree::addElement(std::string str, int _ID)
	{
		addElement(new GUIRectangleTree(str));
		elements.back()->setID(_ID);
	}
	void GUIRectangleTree::open()
	{
		if (checkState(GUIRECT_OPEN))
			return;

		//Close other trees
		if (getRootTree())
			getFirstGenerationParent()->getAsGUIRectangleContainerPtr()->closeTreeElements(getRootTree()->getAsGUIRectanglePtr());

		//Open this
		treeOpenTimer = TREE_OPEN_TIME;
		GUIRectangleContainer::open();
	}
	void GUIRectangleTree::close()
	{
		if (!checkState(GUIRECT_OPEN))
			return;
		
		//Close this
		treeOpenTimer = 0.0f;
		GUIRectangleContainer::close();

		//Close all elements
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->getAsGUIRectangleTreePtr()->close();
	}
	void GUIRectangleTree::toggleOpen()
	{
		if (checkBit(state, GUIRECT_OPEN))
			close();
		else
			open();
	}
	GUIRectangleTree* GUIRectangleTree::getRootTree()
	{
		if (parent && parent->getAsGUIRectangleTreePtr())
			return parent->getAsGUIRectangleTreePtr()->getRootTree();
		return this;
	}
	void GUIRectangleTree::updateMinSize()
	{
		/*Min size is retrieved from rectangle, update it*/
		GUIRectangle::updateMinSize();

		if (isOpen())
		{
			//Min element size
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
		}
	}
	void GUIRectangleTree::onDisable()
	{
		GUIRectangleContainer::onDisable();
		close();
	}
}