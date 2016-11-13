#pragma once
#include "GUIRectangleContainer.h"
#include <vector>

namespace spehs
{
	class GUITextField;
	class GUIRectangleList;
	class GUIWindow : public GUIRectangleContainer
	{
	public:
		//Borders before application window where the window should not be located at
		static int upBorder;
		static int downBorder;
		static int leftBorder;
		static int rightBorder;
		//Strech rectangle width
		static int strechWidth;
		//Strech colouring
		static glm::vec4 strechColorFocused;
		static glm::vec4 strechColorUnfocused;
		//Header colouring
		static glm::vec4 headerBackgroundColor;
		static glm::vec4 headerStringColor;
		//Exit colouring
		static glm::vec4 exitBackgroundColor;
		static glm::vec4 exitStringColor;
	public:
		GUIWindow();
		virtual ~GUIWindow();
		
		virtual void update();
		virtual void postUpdate();
		virtual void updatePosition();
		virtual void updateScale();
		virtual void updateMinSize();//Updates min size variable based on element requirements
		virtual void setRenderState(const bool _state);
		virtual void setDepth(uint16_t);
		virtual void onEnable();
		virtual void onDisable();
		virtual void disableStateRecursive(GUIRECT_STATE_TYPE stateBit);
		virtual void enableStateRecursive(GUIRECT_STATE_TYPE stateBit);
		virtual void disableState(GUIRECT_STATE_TYPE stateBit);
		virtual void enableState(GUIRECT_STATE_TYPE stateBit);

		virtual void close();
		virtual void open();
		virtual void refresh();
		virtual void limitWithinMainWindow();//Reposition window so that it doesn't escape the viewport

		//Getters
		GUIRectangle* getHeaderPtr(){ return header; }
		GUIRectangle* getExitPtr(){ return exit; }
		GUIRectangle* getStrechPtr(){ return strech; }

		//Identity
		GUIWindow* getAsGUIWindowPtr(){ return this; }

	protected:

		GUIRectangle* header;//The header bar, located above the window
		GUIRectangle* exit;//The exit button, located in the top right corner of the window
		GUIRectangle* strech;//Background rectangle, used for streching the window

		int doubleClickTimer;//Timer in milliseconds of how much time is left to carry out a double click
		bool mouseOverStrechArea();//Returns true if mouse is hovering over the strech area
		int8_t strechState;
		
	};
}