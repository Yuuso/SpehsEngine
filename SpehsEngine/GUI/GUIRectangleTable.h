#pragma once
#include "SpehsEngine/GUI/GUIRectangleUnisizeContainer.h"

namespace spehs
{
	/**Stores elements in a nice even cell size table. Column count defines the number of columns*/
	class GUIRectangleTable : public GUIRectangleUnisizeContainer
	{
	public:
		GUIRectangleTable(BatchManager& _batchManager, const int columns);
		~GUIRectangleTable() override;
		
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setColumnCount(const int _columns){ columns = _columns; disableBit(state, GUIRECT_POSITION_UPDATED_BIT); disableBit(state, GUIRECT_SCALE_UPDATED_BIT); }
		int getColumnCount() const { return columns; }

		spehs::GUIRectangleTable* getAsGUIRectangleTablePtr() override { return this; }

	private:
		int columns;
	};
}