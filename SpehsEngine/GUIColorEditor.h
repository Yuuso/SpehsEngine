#pragma once
#include <glm/vec4.hpp>
#include "GUIRectangle.h"
#include "ValueEditor.h"


namespace spehs
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
		GUIColorEditor(const Color color = Color(1.0f, 1.0f, 1.0f, 1.0f));
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

		Polygon* palette;
		Polygon* sliderRG;
		Polygon* sliderGB;
		Polygon* sliderBR;
		Polygon* preview;
		Polygon* alphaSliderBack;
		Polygon* alphaSliderFront;
		float sliderState;
		bool alphaEnabled;
		bool sliderOnHold;
		bool paletteOnHold;
		bool alphaOnHold;
	};
}