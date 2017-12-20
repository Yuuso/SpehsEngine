#pragma once
#include <string>
#include "SpehsEngine/GUI/GUIRectangle.h"
#include "SpehsEngine/GUI/ValueEditor.h"


namespace spehs
{
	class Polygon;
	class GUICheckbox : public GUIRectangle, public ValueEditor<bool>
	{
	public:
		static const spehs::Color defaultCheckboxOuterColor;
		static const spehs::Color defaultCheckboxFillingColor;
	public:
		GUICheckbox(BatchManager& _batchManager);
		~GUICheckbox() override;
		
		void inputUpdate(InputUpdateData& data) override;
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