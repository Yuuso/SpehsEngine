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
		/*Default constructor*/
		GUIRectangleTree();
		/*ID constructor*/
		GUIRectangleTree(int _ID);
		/*Button constructor. Creates button element with given string as the tree rectangle*/
		GUIRectangleTree(std::string str);
		virtual ~GUIRectangleTree();

		void update();
		void render();
		void postRender();
		void updateScale();
		void updatePosition();
		void updateMinSize();
		void loseFocus();
		void setOpenTreeButton(int buttonID){ openTreeButton = buttonID; }
		
		////Elements
		/*Created new node with given string and id*/
		void addElement(std::string str, int _ID);
		void addElement(GUIRectangle* element);
		/*Returns 0 by defualt -> do not use 0 as leaf node index!*/
		int getPressedLeafNodeID(){ return pressedLeafNodeID; }
		void leafNodePressed(int _ID);
		void open();
		void close();
		void toggleOpen();

		GUIRectangleTree* getRootTree();

		//Identity
		GUIRectangleTree* getAsGUIRectangleTreePtr(){ return this; }

	protected:
		int pressedLeafNodeID;//This id is cleared at the beginning of each update. If during that update a leaf node is pressed, its id will be stored in this variable until the next update.
		int openTreeButton;//Referring to input manager key index
		float branchX;//Leaf node x position. If positive, branch towards right, else branch towards left
		int treeOpenTimer;//How long, in milliseconds, the tree remains open after opening it
	};
}