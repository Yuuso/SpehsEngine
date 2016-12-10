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

		void setDepth(int16_t _depth);
		void setRenderState(const bool _state);
		void inputUpdate();
		void visualUpdate();
		void updateMinSize();
		void updateScale();
		void updatePosition();

		void enableAlphaEditing();
		void disableAlphaEditing();
		void toggleAlphaEditing();
		bool colorChanged(){ return stateChanged; }
		glm::vec4 retrieveColor(){ return selectedColor; }

	private:
		bool checkPaletteHover();
		bool checkSliderHover();

		Polygon* palette;
		Polygon* sliderRG;
		Polygon* sliderGB;
		Polygon* sliderBR;
		Polygon* preview;
		Polygon* alphaSlider;
		float sliderState;
		bool alphaEnabled;
		bool sliderOnHold;
		bool paletteOnHold;
		bool stateChanged;
		glm::vec4 selectedColor;
	};
}