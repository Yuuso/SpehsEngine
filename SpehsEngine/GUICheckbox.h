#pragma once
#include <string>
#include "GUIRectangle.h"


namespace spehs
{
	class PolygonBatch;
	class GUICheckbox : public GUIRectangle
	{
	public:
		GUICheckbox();
		GUICheckbox(int _ID);
		GUICheckbox(std::string str);
		~GUICheckbox();

		void update();
		void render();
		void updatePosition();
		void updateScale();
		void updateMinSize();
		void setCheckboxSize(int _size){ checkboxSize = _size; disableStateRecursiveUpwards(GUIRECT_SCALED); }

		bool selectedStateChanged(){ if (checkBit(state, GUIRECT_SELECTED) != previousSelectedState) return true; return false; }
		/*Checkbox updates the boolean behind this address*/
		void setBooleanPtr(bool* ptr);

		//Identity
		GUICheckbox* getAsGUICheckboxPtr(){ return this; }

	protected:
		PolygonBatch* checkboxBackground;
		PolygonBatch* checkboxFilling;
		bool* booleanPtr;
		int checkboxSize;
		bool previousSelectedState;
	};
}