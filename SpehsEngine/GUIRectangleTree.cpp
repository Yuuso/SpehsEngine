#include <iostream>
#include "InputManager.h"
#include "ApplicationData.h"
#include "GUIRectangleTree.h"
#include "GUIRectangle.h"
#include "Time.h"
#define TREE_OPEN_TIME 500


namespace SpehsEngine
{
	GUIRectangleTree::GUIRectangleTree() : pressedLeafNodeID(0), openTreeButton(MOUSE_BUTTON_LEFT)
	{
		disableBit(state, GUIRECT_OPEN);
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
				if (getMouseHoverContainer() && getFirstGenerationParent()->checkState(GUIRECT_FOCUSED))
					open();
				else if (treeOpenTimer <= 0)
					close();
			}

		}


		pressedLeafNodeID = 0;//Reset pressed leaf node id before next node update

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
		if (checkBit(state, GUIRECT_MOUSE_HOVER_CONTAINER))
		{//Mouse hovering over the container
			treeOpenTimer = TREE_OPEN_TIME;
		}
		else if (treeOpenTimer > 0)
		{//If mouse is outside the container, close over time
			treeOpenTimer -= deltaTime;
			if (treeOpenTimer <= 0)
				close();
		}
	}
	void GUIRectangleTree::render()
	{
		GUIRectangleContainer::render();
	}
	void GUIRectangleTree::postRender()
	{
		if (isOpen())
		{
			for (int i = beginElementIndex; i < beginElementIndex + updateElementCount; i++)
				elements[i]->postRender();
		}
		else
			GUIRectangle::postRender();
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
			////Element positioning
			if (parent->getAsGUIRectangleTreePtr())
			{//Parent tree exists, mimic parent branch direction
				if (parent->getAsGUIRectangleTreePtr()->branchX > 0)
					branchX = size.x;//Parent branches right
				else
					branchX = -minElementSize.x;//Parent branches left
			}
			else
			{//Parent TREE doesn't exist. Decide branching direction based on horizontal location/size
				if (getX() + size.x / 2.0f > applicationData->getWindowWidthHalf())
				{//Branch left
					if (getFirstGenerationParent() != this)
						branchX = getFirstGenerationParent()->getX() - getX() - minElementSize.x;
					else
						branchX = -minElementSize.x;
				}
				else
				{//Branch right
					if (getFirstGenerationParent())
						branchX = size.x;//may need to be implemented in the future
					else
						branchX = size.x;
				}
			}

			//Position elements
			for (int i = beginElementIndex; i <= getEndElementIndex(); i++)
			{
				if (i == beginElementIndex)
					elements[i]->setPosition(branchX, 0);
				else
					elements[i]->setPosition(branchX, elements[i - 1]->getLocalY() + elements[i - 1]->getHeight());
			}
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
		
		if (_ID > 1000000)//DEBUG
			std::cout << "Inform Teo";//DEBUG
	}
	void GUIRectangleTree::addElement(GUIRectangle* element)
	{
		GUIRectangleContainer::addElement(element);

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
		if (isOpen())
			return;

		//Close other trees
		if (getRootTree())
			getFirstGenerationParent()->getAsGUIRectangleContainerPtr()->closeTreeElements(getRootTree()->getAsGUIRectanglePtr());

		GUIRectangleContainer::open();
	}
	void GUIRectangleTree::close()
	{
		if (!isOpen())
			return;

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
	void GUIRectangleTree::loseFocus()
	{
		GUIRectangleContainer::loseFocus();
		close();
	}
}