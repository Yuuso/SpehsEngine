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
		GUICheckbox(const GUIRECT_ID_TYPE _ID);
		GUICheckbox(const std::string str);
		~GUICheckbox() override;
		
		void inputUpdate() override;
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;
		void setRenderState(const bool _state) override;
		void setDepth(const int16_t depth) override;

		void setCheckboxSize(const int _size){ checkboxSize = _size; disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT); }
		void setEditorState(const bool _state){ selectedState = _state; previousSelectedState = _state; }
		bool valueEdited() const;
		bool retrieveEditorState() const { return selectedState; }

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