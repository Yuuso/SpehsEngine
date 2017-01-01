#pragma once
#include <string>
#include "GUIRectangle.h"
#include "ValueEditor.h"


namespace spehs
{
	class Polygon;
	class GUICheckbox : public GUIRectangle, public ValueEditor<bool>
	{
	public:
		GUICheckbox(const bool checkboxStateEnabled);
		GUICheckbox(const GUIRECT_ID_TYPE _ID);
		GUICheckbox(const char* str);
		~GUICheckbox() override;
		
		void inputUpdate() override;
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;
		void setRenderState(const bool _state) override;
		void setDepth(const int16_t depth) override;

		void setCheckboxSize(const int _size){ checkboxSize = _size; disableStateRecursiveUpwards(GUIRECT_SCALE_UPDATED_BIT); }

		//Identity
		GUICheckbox* getAsGUICheckboxPtr(){ return this; }

	protected:
		void onEditorValueChange() override;

		Polygon* checkboxBackground;
		Polygon* checkboxFilling;
		int checkboxSize;
	};
}