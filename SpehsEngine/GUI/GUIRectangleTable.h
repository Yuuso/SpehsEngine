#pragma once
#include "SpehsEngine/GUI/GUIRectangleUnisizeContainer.h"

namespace spehs
{
	/**Stores elements in a nice even cell size table. Column count defines the number of columns*/
	class GUIRectangleTable : public GUIRectangleUnisizeContainer
	{
	public:
		GUIRectangleTable(GUIContext& context, const int columns);
		~GUIRectangleTable() override;
		
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setColumnCount(const size_t _columns){ columns = _columns; disableBit(state, GUIRECT_POSITION_UPDATED_BIT); disableBit(state, GUIRECT_SCALE_UPDATED_BIT); }
		size_t getColumnCount() const { return columns; }

		spehs::GUIRectangleTable* getAsGUIRectangleTablePtr() override { return this; }

	private:
		size_t columns;
	};
}