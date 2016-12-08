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
		GUIRectangleList();
		GUIRectangleList(uint32_t);
		~GUIRectangleList();

		//Overriding methods
		void setRenderState(const bool _state);
		void inputUpdate();
		void visualUpdate();
		void updateMinSize();
		void updateScale();
		void updatePosition();
		void setDepth(int16_t depth);
		void addElement(GUIRectangle* e);
		void clear();

		/*Returns index to the first visible element on the element list*/
		int getBeginElementIndex(){ return beginElementIndex; }
		/*Returns index to the last visible element on the list*/
		int getUpdateElementCount(){ return updateElementCount; }
		virtual void incrementUpdateElementCount(int incrementation);
		bool invisibleElements(){ if (elements.size() > updateElementCount) return true; return false; }
		void scroll(int amount);
		/*Updates update element count based on current dimensions. Element size is also updated.*/
		virtual void updateUpdateElementCount();
		
		//Identity
		GUIRectangleList* getAsGUIRectangleListPtr(){ return this; }

	protected:
		GUIRectangle* scrollUp;
		GUIRectangle* scrollBar;
		GUIRectangle* scrollDown;
		int beginElementIndex;///<Index of the first updated/rendered element
		int updateElementCount;///<Amount of elements updated/rendered beginning from begin element index
	};
}