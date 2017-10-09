#pragma once
#include "GUIRectangleUnisizeContainer.h"
#include <vector>

/*
Elements are always in min element size, freely going ot of the main window if enough elements are present.
*/

namespace spehs
{
	class GUIRectangleTree : public GUIRectangleUnisizeContainer
	{
	public:
		static time::Time defaultTreeOpenTime;
	public:
		GUIRectangleTree();
		GUIRectangleTree(const int _ID);
		GUIRectangleTree(const std::string& str);
		virtual ~GUIRectangleTree();

		void inputUpdate(InputUpdateData& data);
		void updateMinSize();
		void updateScale();
		void updatePosition();
		void setOpenTreeButton(const int buttonID){ openTreeButton = buttonID; }
		void setTreeOpenTime(const float seconds){ treeOpenTime = time::seconds(seconds); }
		bool open();
		bool close();			
		void addElement(const std::string& str, const GUIRECT_ID_TYPE _ID);///< Creates new node with given string and id
		void addElement(GUIRectangle* element);

		//Tree
		int getPressedLeafNodeID(){ return pressedLeafNodeID; }
		void leafNodePressed(const GUIRECT_ID_TYPE _ID);///< Sets pressed leaf node recursively upwards
		GUIRectangleTree* getRootTree();

		//Identity
		GUIRectangleTree* getAsGUIRectangleTreePtr() override { return this; }

	protected:
		void onDisableInput() override;

		int pressedLeafNodeID;//This id is cleared at the beginning of each update. If during that update a leaf node is pressed, its id will be stored in this variable until the next update.
		unsigned openTreeButton;//Referring to input manager key index
		float branchX;//Leaf node x position. If positive, branch towards right, else branch towards left
		time::Time treeOpenTimer;//How long, in milliseconds, the tree remains open after opening it
		time::Time treeOpenTime;
	};
}