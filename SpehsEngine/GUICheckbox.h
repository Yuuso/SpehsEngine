#pragma once

#include "GUIRectangle.h"

#include <string>


namespace spehs
{
	class Polygon;
	class GUICheckbox : public GUIRectangle
	{
	public:
		GUICheckbox();
		GUICheckbox(int _ID);
		GUICheckbox(std::string str);
		~GUICheckbox();
		
		void inputUpdate();

		void updateMinSize();
		void updateScale();
		void updatePosition();

		void setRenderState(const bool _state);
		void setDepth(int16_t depth);
		void setCheckboxSize(int _size){ checkboxSize = _size; disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT); }

		bool selectedStateChanged();
		/*Checkbox updates the boolean behind this address*/
		void setBooleanPtr(bool* ptr);

		//Identity
		GUICheckbox* getAsGUICheckboxPtr(){ return this; }

	protected:
		Polygon* checkboxBackground;
		Polygon* checkboxFilling;
		bool* booleanPtr;
		int checkboxSize;
		bool previousSelectedState;
	};
}