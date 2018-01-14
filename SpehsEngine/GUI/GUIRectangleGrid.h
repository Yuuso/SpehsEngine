#pragma once
#include "SpehsEngine/GUI/GUIRectangleUnisizeContainer.h"

namespace spehs
{
	class GUIRectangleGrid : public GUIRectangleUnisizeContainer
	{
	public:
		static unsigned defaultBorderWidth;
	public:
		GUIRectangleGrid(GUIContext& context);
		~GUIRectangleGrid();

		void setRenderState(const bool _state) override;
		void setDepth(const int16_t depth) override;
		void inputUpdate() override;
		void visualUpdate() override;
		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;
		void scroll(const int amount);
		void setGridBorderWidth(const unsigned width);
		int getPreferredHeight() override;

	protected:
		int getRowCount();
		int getMaxScrollState();

		int scrollBarWidth;
		int scrollState;
		unsigned borderWidth;
		bool scrollingEnabled;
		bool draggingScrollBar;
		float scrollBarAccumulatedDragY;
		spehs::GUIRectangle* scrollUp;
		spehs::GUIRectangle* scrollBar;
		spehs::GUIRectangle* scrollDown;
	};
}