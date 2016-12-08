#pragma once
#include "GUIRectangleContainer.h"
#include <vector>

/*
Elements are always in min element size, freely going ot of the main window if enough elements are present.
*/

namespace spehs
{
	class GUIRectangleTree : public GUIRectangleContainer
	{
	public:
		GUIRectangleTree();
		GUIRectangleTree(int _ID);
		GUIRectangleTree(std::string str);
		virtual ~GUIRectangleTree();

		void inputUpdate();
		void updateMinSize();
		void updateScale();
		void updatePosition();
		void setOpenTreeButton(int buttonID){ openTreeButton = buttonID; }
		bool open();
		bool close();			
		void addElement(std::string str, int _ID);///< Creates new node with given string and id
		void addElement(GUIRectangle* element);

		//Tree
		int getPressedLeafNodeID(){ return pressedLeafNodeID; }
		void leafNodePressed(int _ID);///< Sets pressed leaf node recursively upwards
		GUIRectangleTree* getRootTree();

		//Identity
		GUIRectangleTree* getAsGUIRectangleTreePtr(){ return this; }

	protected:
		void onDisable();

		int pressedLeafNodeID;//This id is cleared at the beginning of each update. If during that update a leaf node is pressed, its id will be stored in this variable until the next update.
		unsigned openTreeButton;//Referring to input manager key index
		float branchX;//Leaf node x position. If positive, branch towards right, else branch towards left
		int treeOpenTimer;//How long, in milliseconds, the tree remains open after opening it
	};
}