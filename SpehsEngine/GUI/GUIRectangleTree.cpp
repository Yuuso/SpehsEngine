#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/GUI/GUIRectangleTree.h"
#include "SpehsEngine/GUI/GUIRectangle.h"

namespace se
{
	time::Time GUIRectangleTree::defaultTreeOpenTime = time::fromSeconds(0.5f);
	GUIRectangleTree::GUIRectangleTree(GUIContext& context)
		: GUIRectangleUnisizeContainer(context)
		, openTreeButton(MOUSE_BUTTON_LEFT)
		, treeOpenTime(defaultTreeOpenTime)
	{
		close();
	}

	GUIRectangleTree::~GUIRectangleTree()
	{
	}

	void GUIRectangleTree::inputUpdate()
	{
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
				if (getMouseHover() && getInputManager().isKeyPressed(openTreeButton))
				{//If button is a tree and was pressed...
					if (elements.size() == 0)//If this is button tree, must not have nodes underneath to trigger leaf node press
						leafNodePressed(id);
					else//Contains nodes, toggle open
						toggleOpen();
				}
			}
			else
			{//Open when mouse is hovering over
				if (checkState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT) && getInputEnabled())
					open();
				else if (treeOpenTimer <= time::Time::zero)
					close();
			}
		}

		if (checkState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT) && isOpen())
		{//Mouse hovering over the container
			treeOpenTimer = treeOpenTime;
		}
		else if (treeOpenTimer > se::time::zero)
		{//If mouse is outside the container, close over time
			treeOpenTimer -= getDeltaTimeSystem().deltaTime;
			if (treeOpenTimer <= time::Time::zero)
				close();
			else
				enableState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT);//Keep container hover active for the duration of open timer
		}
	}

	void GUIRectangleTree::updateMinSize()
	{
		//Update uniform element size
		GUIRectangleUnisizeContainer::updateMinSize();

		//Min size is retrieved from rectangle, update it
		GUIRectangle::updateMinSize();
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
				if (firstGenParent->getXGlobal() + firstGenParent->getWidth() / 2.0f > getWindow().getWidth() / 2)
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
			if (branchY + int(elements.size()) * minElementSize.y + 1 > getWindow().getHeight())
				branchY = getWindow().getHeight() - int(elements.size()) * minElementSize.y - 1;

			//Position elements
			for (unsigned i = 0; i < elements.size(); i++)
				elements[i]->setPositionGlobal(getXGlobal() + branchX, branchY + i * minElementSize.y);
		}
	}

	void GUIRectangleTree::leafNodePressed(const GUIRECT_ID_TYPE _ID)
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
	}

	void GUIRectangleTree::addElement(const std::string& str, const GUIRECT_ID_TYPE _ID)
	{
		addElement(new GUIRectangleTree(getGUIContext()));
		elements.back()->setString(str);
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
		treeOpenTimer = treeOpenTime;
		return true;
	}

	bool GUIRectangleTree::close()
	{
		if (!GUIRectangleContainer::close())
			return false;
		
		//Close timer reset
		treeOpenTimer = time::Time::zero;

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