#pragma once
#include "GUIRectangleContainer.h"

namespace SpehsEngine
{
	/**Stores elements in a nice even cell size table. Column count defines the number of columns*/
	class GUIRectangleTable : public GUIRectangleContainer
	{
	public:
		GUIRectangleTable(int columns);
		~GUIRectangleTable();
		
		void updatePosition();
		void updateScale();
		void updateMinSize();

		void setColumnCount(int _columns){ columns = _columns; disableBit(state, GUIRECT_POSITIONED); disableBit(state, GUIRECT_SCALED); }
		int getColumnCount(){ return columns; }

	private:
		int columns;
	};
}