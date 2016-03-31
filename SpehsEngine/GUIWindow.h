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
		GUIWindow();
		virtual ~GUIWindow();
		
		virtual void update();
		virtual void updatePosition();
		virtual void updateScale();
		virtual void updateMinSize();//Updates min size variable based on element requirements
		virtual void setRenderState(const bool _state);
		virtual void setPostRenderState(const bool _state);

		virtual void gainFocus();
		virtual void loseFocus();
		virtual void disableStateRecursive(GUIRECT_STATE_TYPE stateBit);
		virtual void enableStateRecursive(GUIRECT_STATE_TYPE stateBit);
		void disableState(GUIRECT_STATE_TYPE stateBit);
		void enableState(GUIRECT_STATE_TYPE stateBit);

		virtual void close();
		virtual void open();
		virtual void refresh();
		void limitWithinMainWindow();//Reposition window so that it doesn't escape the viewport

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