#pragma once
#include <vector>
#include "SpehsEngine/GUI/GUIRectangleUnisizeContainer.h"
#define SCROLLBAR_CLICK_THRESHOLD 5


namespace se
{
	/**
	A similar container class to the GUIRectangleColumn container,
	ScrollList will automatically limit its size and hide elements when necessary.
	When elements are hidden, scroll bar functionality is added for the user to interact.
	*/
	class GUIRectangleScrollList : public GUIRectangleUnisizeContainer
	{
	public:
		static int defaultScrollBarWidth;
		static std::string scrollUpTexturePath;
		static std::string scrollBarTexturePath;
		static std::string scrollDownTexturePath;
		static se::Color defaultScrollElementColor;
	public:
		GUIRectangleScrollList(GUIContext& context);
		~GUIRectangleScrollList() override;

		//Overriding methods
		void setRenderState(const bool _state) override;
		void inputUpdate() override;
		void visualUpdate() override;
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;
		void setDepth(const int16_t depth) override;
		GUIRectangle* addElement(GUIRectangle* e) override;
		bool moveElement(GUIRectangle* element, const size_t targetIndexBeforeMove) override;
		void clear() override;
		bool open() override;
		bool close() override;
		bool removeElement(GUIRectangle* element) override;
		int getPreferredHeight() override;

		/*Returns index to the first visible element on the element list*/
		size_t getBeginElementIndex() const { return beginElementIndex; }
		/*Returns index to the last visible element on the list*/
		size_t getUpdateElementCount() const { return updateElementCount; }
		/*Relative addition to the number of elements updated*/
		virtual void incrementUpdateElementCount(int incrementation);
		/*The list cannot be minimized below the size required to fit this many elements*/
		void setMinVisibleElementCount(const size_t count);///< Min visible element count must be at least 4 because of how the scroll bar is aligned
		inline size_t getMinVisibleElementCount() const { return minVisibleElementCount; }
		/*Returns true if any amount of elements are not being displayed at the moment*/
		bool invisibleElements() const { return elements.size() > updateElementCount; }
		/*Scrolls the list in either direction*/
		void scroll(int amount);
		
		//Identity
		GUIRectangleScrollList* getAsGUIRectangleScrollListPtr() override { return this; }

	protected:

		GUIRectangle* scrollUp = nullptr;
		GUIRectangle* scrollBar = nullptr;
		GUIRectangle* scrollDown = nullptr;
		size_t beginElementIndex = 0;///< Index of the first updated/rendered element
		size_t updateElementCount = 0;///< Amount of elements updated/rendered beginning from begin element index
		size_t minVisibleElementCount = 4;
	};
}