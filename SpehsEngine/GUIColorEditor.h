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
		void setEditorColor(glm::vec4 _color){ selectedColor = _color; }

	private:
		bool checkPaletteHover();
		bool checkSliderHover();
		bool checkAlphaHover();

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