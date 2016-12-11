#pragma once
#include <string>
#include "GUIEditor.h"


namespace spehs
{
	class Polygon;
	class GUICheckbox : public GUIEditor
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

		void setEditorState(const bool _state){ selectedState = _state; previousSelectedState = _state; }
		bool valueEdited();
		bool retrieveEditorState(){ return selectedState; }

		//Identity
		GUICheckbox* getAsGUICheckboxPtr(){ return this; }

	protected:
		Polygon* checkboxBackground;
		Polygon* checkboxFilling;
		int checkboxSize;
		bool selectedState;
		bool previousSelectedState;
	};
}