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
		GUIRectangleRow(GUIRECT_ID_TYPE _id);
		~GUIRectangleRow();

		void updatePosition();
		void updateScale();
		void updateMinSize();
		void setEvenElementWidth(bool setting);
		void setElementPositionMode(PositionMode mode);
		
		//Identity
		GUIRectangleRow* getAsGUIRectangleRowPtr(){ return this; }
	private:
		bool evenElementWidth;
		PositionMode elementPositionMode;
	};
}