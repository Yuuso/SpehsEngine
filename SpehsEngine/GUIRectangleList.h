#pragma once
#include <vector>
#include "GUIRectangleContainer.h"
#define SCROLL_BUTTON_WIDTH 20
#define SCROLLBAR_CLICK_THRESHOLD 5


namespace spehs
{
	class GUIRectangleList : public GUIRectangleContainer
	{
	public:
		virtual void DRAW_TEXT();

		GUIRectangleList();
		GUIRectangleList(uint32_t);
		virtual ~GUIRectangleList();

		virtual void setRenderState(const bool _state);
		virtual void update();
		virtual void postUpdate();
		virtual void updateScale();
		virtual void updatePosition();
		virtual void updateMinSize();
		virtual void incrementUpdateElementCount(int incrementation);
		/*Updates update element count based on current dimensions. Element size is also updated.*/
		virtual void updateUpdateElementCount();
		virtual void setDepth(uint16_t depth);


		//Element management
		virtual void addElement(GUIRectangle* e);

		//Identity
		GUIRectangleList* getAsGUIRectangleListPtr(){ return this; }

	protected:
		GUIRectangle* scrollUp;
		GUIRectangle* scrollBar;
		GUIRectangle* scrollDown;
	};
}