#pragma once
#include "GUIRectangle.h"


namespace spehs
{
	/** retriveColor() method returns the color being edited at its latest state. */
	class GUIColorEditor : public GUIRectangle
	{
	public:
		static float paletteWidth;
		static float sliderWidth;
		static float colorPreviewWidth;
		static int colorEditorBorder;
	public:
		GUIColorEditor();
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
		bool colorChanged() const { return stateChanged; }
		glm::vec4 retrieveColor() const { return selectedColor; }
		void setEditorColor(const glm::vec4 _color){ selectedColor = _color; }

	private:
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
		bool stateChanged;
		glm::vec4 selectedColor;
	};
}