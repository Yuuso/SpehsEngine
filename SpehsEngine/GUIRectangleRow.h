#pragma once
#include "GUIRectangleContainer.h"

namespace spehs
{
	/*A class for storing multiple gui rectangle elements in a row, all remaining visible*/
	class GUIRectangleRow : public GUIRectangleContainer
	{
	public:
		enum class PositionMode
		{
			Standard, StackLeft, StackRight, StackEdges
		};

		GUIRectangleRow();
		GUIRectangleRow(const GUIRECT_ID_TYPE _id);
		~GUIRectangleRow() override;

		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setEvenElementWidth(const bool setting);
		void setElementPositionMode(const PositionMode mode);///< Elements can be "stacked" to one end of the row, only edges or none at all (If possible, elements in stacked area are given their minimum width requirement)
		
		//Identity
		GUIRectangleRow* getAsGUIRectangleRowPtr() override { return this; }
	private:
		bool evenElementWidth;
		PositionMode elementPositionMode;
	};
}