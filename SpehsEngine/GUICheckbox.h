#pragma once
#include <string>
#include "GUIRectangle.h"


namespace SpehsEngine
{
	class PolygonBatch;
	class GUICheckbox : public GUIRectangle
	{
	public:
		GUICheckbox();
		GUICheckbox(int _ID);
		GUICheckbox(std::string str);
		~GUICheckbox();

		void update();
		void render();
		void updatePosition();
		void updateScale();
		void updateMinSize();

		/*Checkbox updates the boolean behind this address*/
		void setBooleanPtr(bool* ptr);

		//Identity
		GUICheckbox* getAsGUICheckboxPtr(){ return this; }

	protected:
		PolygonBatch* checkboxBackground;
		PolygonBatch* checkboxFilling;
		bool* booleanPtr;
		float checkboxWidth;
	};
}