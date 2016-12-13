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
	void GUIRectangleTree::inputUpdate()
	{//NOTE: do not use container update, tree requires special function call ordering
		
		pressedLeafNodeID = 0;
		GUIRectangleContainer::inputUpdate();
		
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
				if (checkState(GUIRECT_MOUSE_HOVER_CONTAINER) && checkState(GUIRECT_INPUT_ENABLED_BIT))
				{
					open();
					if (getString() == "def")
					{
						int der(0);
						der++;
					}
				}
				else if (treeOpenTimer <= 0)
					close();
			}
		}

		if (checkState(GUIRECT_MOUSE_HOVER_CONTAINER))
		{//Mouse hovering over the container
			treeOpenTimer = TREE_OPEN_TIME;
		}
		else if (treeOpenTimer > 0)
		{//If mouse is outside the container, close over time
			treeOpenTimer -= time::getDeltaTimeAsMilliseconds();
			if (treeOpenTimer <= 0)
				close();
			else
				enableState(GUIRECT_MOUSE_HOVER_CONTAINER);//Keep container hover active for the duration of open timer
		}
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
			if (branchY + elements.size() * minElementSize.y + 1 > applicationData->getWindowHeight())
				branchY = applicationData->getWindowHeight() - elements.size() * minElementSize.y - 1;

			//Position elements
			for (unsigned i = 0; i < elements.size(); i++)
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
		element->setRenderState(checkState(GUIRECT_OPEN_BIT) && getRenderState());

		//By default, make tree to open on mouse hover when node element is added
		openTreeButton = 0;
	}
	void GUIRectangleTree::addElement(std::string str, int _ID)
	{
		addElement(new GUIRectangleTree(str));
		elements.back()->setID(_ID);
	}
	bool GUIRectangleTree::open()
	{
		if (GUIRectangleContainer::open())
			return false;

		//Close other trees
		if (getRootTree())
			getFirstGenerationParent()->closeElementsOfType<GUIRectangleTree>(this);

		//Open this
		treeOpenTimer = TREE_OPEN_TIME;
		return true;
	}
	bool GUIRectangleTree::close()
	{
		if (!GUIRectangleContainer::close())
			return false;
		
		//Close timer reset
		treeOpenTimer = 0.0f;

		//Close all elements
		for (unsigned i = 0; i < elements.size(); i++)
			elements[i]->getAsGUIRectangleTreePtr()->close();

		return true;
	}
	GUIRectangleTree* GUIRectangleTree::getRootTree()
	{
		if (parent && parent->getAsGUIRectangleTreePtr())
			return parent->getAsGUIRectangleTreePtr()->getRootTree();
		return this;
	}
	void GUIRectangleTree::onDisableInput()
	{
		GUIRectangleContainer::onDisableInput();
		close();
	}
}