#pragma once
#include <string>
#include "SpehsEngine/GUI/GUIRectangle.h"
#include "SpehsEngine/GUI/ValueEditor.h"


namespace se
{
	namespace rendering
	{
		class Polygon;
	}
	class GUICheckbox : public GUIRectangle, public ValueEditor<bool>
	{
	public:
		static const se::Color defaultCheckboxOuterColor;
		static const se::Color defaultCheckboxFillingColor;
	public:
		GUICheckbox(GUIContext& context);
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

		se::rendering::Polygon* checkboxBackground;
		se::rendering::Polygon* checkboxFilling;
		int checkboxSize;
	};
}