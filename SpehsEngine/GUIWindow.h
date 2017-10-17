#pragma once
#include "GUIRectangleContainer.h"
#include <vector>
#define GUIWINDOW_BASE_ELEMENT_COUNT 3

namespace spehs
{
	class GUIStringEditor;
	class GUIRectangleScrollList;
	class GUIWindow : public GUIRectangleContainer
	{
	public:
		friend class GUIWindowManager;
		//Borders before application window where the window should not be located at
		static int upBorder;
		static int downBorder;
		static int leftBorder;
		static int rightBorder;
		//Strech rectangle width
		static int strechWidth;
		//Strech colouring
		static spehs::Color strechColorFocused;
		static spehs::Color strechColorUnfocused;
		//Header colouring
		static spehs::Color headerBackgroundColor;
		static spehs::Color headerStringColor;
		//Exit colouring
		static spehs::Color exitBackgroundColor;
		static spehs::Color exitStringColor;

		enum class PositionMode { StackUp, StackDown, Spread };
	public:
		GUIWindow(BatchManager& _batchManager);
		~GUIWindow() override;
		
		void inputUpdate(InputUpdateData& data) override;
		void visualUpdate() override;
		void updateMinSize() override;//Updates min size variable based on element requirements
		void updateScale() override;
		void updatePosition() override;
		void setDepth(const int16_t) override;
		void onEnableInput() override;
		void onDisableInput() override;
		void clear() override;

		bool close() override;
		bool open() override;
		virtual void limitWithinMainWindow();//Reposition window so that it doesn't escape the viewport
		virtual void requestRefresh();//Request refresh within the next loop cycle
		inline int getRefreshRequestCount() const { return refreshRequests; }
		inline bool isDragging() const { return (state & GUIWINDOW_DRAGGING_BIT) != 0; }
		inline bool isStreching() const { return (state & GUIWINDOW_STRECHING_BIT) != 0; }
		bool isReceivingInput() const override;
		void setPositionMode(const PositionMode mode);

		//Getters
		GUIRectangle* getHeaderPtr(){ return header; }
		GUIRectangle* getExitPtr(){ return exit; }
		GUIRectangle* getStrechPtr(){ return strech; }

		//Identity
		GUIWindow* getAsGUIWindowPtr() override { return this; }

	protected:
		virtual void refresh();//Elements must not have direct access to refresh!

		GUIRectangle* header;//The header bar, located above the window
		GUIRectangle* exit;//The exit button, located in the top right corner of the window
		GUIRectangle* strech;//Background rectangle, used for streching the window

		time::Time doubleClickTimer;//Timer in milliseconds of how much time is left to carry out a double click
		bool mouseOverStrechArea();//Returns true if mouse is hovering over the strech area
		int8_t strechState;
		int refreshRequests;
		PositionMode positionMode;
	};
}