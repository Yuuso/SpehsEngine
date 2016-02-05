#pragma once
#include "GUIRectangleContainer.h"

namespace SpehsEngine
{
	/*A class for storing multiple gui rectangle elements in a row, all remaining visible*/
	class GUIRectangleRow : public GUIRectangleContainer
	{
	public:
		GUIRectangleRow();
		~GUIRectangleRow();
		friend class GUIRectangleGrid;

		void updatePosition();
		void updateScale();
		void updateMinSize();
		void setEvenElementWidth(bool setting);
		
		//Identity
		GUIRectangleRow* getAsGUIRectangleRowPtr(){ return this; }
	private:
		bool evenElementWidth;

	};
}