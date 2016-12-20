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
			Left, Right, Standard
		};

		GUIRectangleRow();
		GUIRectangleRow(const GUIRECT_ID_TYPE _id);
		~GUIRectangleRow() override;

		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setEvenElementWidth(const bool setting);
		void setElementPositionMode(const PositionMode mode);
		
		//Identity
		GUIRectangleRow* getAsGUIRectangleRowPtr() override { return this; }
	private:
		bool evenElementWidth;
		PositionMode elementPositionMode;
	};
}