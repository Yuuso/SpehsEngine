#pragma once
#include "SpehsEngine/GUI/GUIRectangle.h"
#include "SpehsEngine/GUI/ValueEditor.h"
#include "SpehsEngine/Core/Color.h"


namespace se
{
	/** retriveColor() method returns the color being edited at its latest state. */
	class GUIColorEditor : public GUIRectangle, public ValueEditor<Color>
	{
	public:
		static float paletteWidth;
		static float sliderWidth;
		static float colorPreviewWidth;
		static int colorEditorBorder;
	public:
		GUIColorEditor(GUIContext& context);
		~GUIColorEditor();

		void setDepth(const int16_t _depth) override;
		void setRenderState(const bool _state) override;
		void inputUpdate() override;
		void visualUpdate() override;
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void enableAlphaEditing();
		void disableAlphaEditing();
		void toggleAlphaEditing();

	private:
		void onEditorValueChange() override;
		bool checkPaletteHover() const;
		bool checkSliderHover() const;
		bool checkAlphaHover()const;

		se::rendering::Polygon* palette;
		se::rendering::Polygon* sliderRG;
		se::rendering::Polygon* sliderGB;
		se::rendering::Polygon* sliderBR;
		se::rendering::Polygon* preview;
		se::rendering::Polygon* alphaSliderBack;
		se::rendering::Polygon* alphaSliderFront;
		float sliderState;
		bool alphaEnabled;
		bool sliderOnHold;
		bool paletteOnHold;
		bool alphaOnHold;
	};
}