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
		GUIRectangleList(const GUIRECT_ID_TYPE id);
		~GUIRectangleList() override;

		//Overriding methods
		void setRenderState(const bool _state) override;
		void inputUpdate() override;
		void visualUpdate() override;
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;
		void setDepth(const int16_t depth) override;
		void addElement(GUIRectangle* e) override;
		void clear() override;

		/*Returns index to the first visible element on the element list*/
		int getBeginElementIndex() const { return beginElementIndex; }
		/*Returns index to the last visible element on the list*/
		int getUpdateElementCount() const { return updateElementCount; }
		virtual void incrementUpdateElementCount(int incrementation);
		bool invisibleElements() const { if (elements.size() > updateElementCount) return true; return false; }
		void scroll(int amount);
		/*Updates update element count based on current dimensions. Element size is also updated.*/
		virtual void updateUpdateElementCount();
		
		//Identity
		GUIRectangleList* getAsGUIRectangleListPtr() override { return this; }

	protected:
		GUIRectangle* scrollUp;
		GUIRectangle* scrollBar;
		GUIRectangle* scrollDown;
		int beginElementIndex;///<Index of the first updated/rendered element
		int updateElementCount;///<Amount of elements updated/rendered beginning from begin element index
	};
}