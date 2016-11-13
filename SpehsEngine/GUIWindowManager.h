#pragma once
#include <vector>

namespace spehs
{
	class GUIPopup;
	class GUIWindow;
	class Polygon;
	///Windows are stored in a vector. This vector may change ordering based on performed actions, therefore external for loops and such should be used very carefully!
	class GUIWindowManager
	{
	public:
		GUIWindowManager();
		~GUIWindowManager();

		void update();
		void addWindow(GUIWindow* window);
		void addPopup(GUIPopup* popup);
		void refreshWindows();
		GUIWindow* getFocusedWindow(){ return focusedWindow; }
		void toggleWindow(GUIWindow* window);
		void openWindow(GUIWindow* window);
		void closeWindow(GUIWindow* window);//Does not modify window order
		bool receivingInput();
		bool getMouseHoverAny();/// Returns true if any window is under the mouse
		unsigned getWindowCount(){ return windows.size(); }
		unsigned getPopupCount(){ return popups.size(); }
		GUIWindow* back(){ return windows.back(); }
		GUIWindow* at(int index){ if (index < 0 || index >= windows.size()) return nullptr; return windows[index]; }
		void setSystemDepth(uint16_t newDepth);
		void setPopupShadeColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

	private:
		void updateDepths();

		GUIWindow* focusedWindow;
		std::vector<GUIWindow*> windows;
		std::vector<GUIPopup*> popups;
		uint16_t systemDepth;
		Polygon* popupShade;
		float popupShadeAlpha;
	};
}