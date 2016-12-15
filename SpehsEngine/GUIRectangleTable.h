#pragma once
#include "GUIRectangleContainer.h"

namespace spehs
{
	/**Stores elements in a nice even cell size table. Column count defines the number of columns*/
	class GUIRectangleTable : public GUIRectangleContainer
	{
	public:
		GUIRectangleTable(int columns);
		~GUIRectangleTable();
		
		void updateMinSize();
		void updateScale();
		void updatePosition();

		void setColumnCount(int _columns){ columns = _columns; disableBit(state, GUIRECT_POSITION_UPDATED_BIT); disableBit(state, GUIRECT_SCALE_UPDATED_BIT); }
		int getColumnCount(){ return columns; }

	private:
		int columns;
	};
}