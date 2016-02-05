#pragma once
#include "GUIRectangleContainer.h"

namespace SpehsEngine
{
	/*
	Inherited from gui rectangle list for scrolling purposes
	Elements are stored in rectangle rows (rows are located in elements vector)
	Element pointers have temporal storeage in grid elements vector
	*/
	class GUIRectangleGrid : public GUIRectangleContainer
	{
	public:
		GUIRectangleGrid();
		~GUIRectangleGrid();

		void updateScale();
		void updatePosition();
		void updateMinSize();
		
		void clear();
		
		//Identity
		GUIRectangleGrid* getAsGUIRectangleGridPtr(){ return this; }

	protected:
		void updateGridDimensions();
		int rowCount;
		int columnCount;
		bool squareShaped;
	};
}