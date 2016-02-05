#pragma once
#include <vector>
#include "GUIRectangleContainer.h"
#define SCROLL_BUTTON_WIDTH 20
#define SCROLLBAR_CLICK_THRESHOLD 5


namespace SpehsEngine
{
	class GUIRectangleList : public GUIRectangleContainer
	{
	public:
		GUIRectangleList();
		GUIRectangleList(uint32_t);
		~GUIRectangleList();

		void render();
		void postRender();
		void update();
		void updateScale();
		void updatePosition();
		void updateMinSize();
		void incrementUpdateElementCount(int incrementation);
		/*Updates update element count based on current dimensions. Element size is also updated.*/
		virtual void updateUpdateElementCount();

		//Element management
		void addElement(GUIRectangle* e);

		//Identity
		GUIRectangleList* getAsGUIRectangleListPtr(){ return this; }

	protected:
		GUIRectangle* scrollUp;
		GUIRectangle* scrollBar;
		GUIRectangle* scrollDown;
	};
}