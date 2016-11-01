#pragma once
#include <vector>

namespace spehs
{
	class GUIPopup;
	class GUIWindow;
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
		void setFocusedWindow(GUIWindow* window);
		void toggleWindow(GUIWindow* window);
		void openWindow(GUIWindow* window);
		void closeWindow(GUIWindow* window);
		bool receivingInput();
		bool getMouseHoverAny();/// Returns true if any window is under the mouse
		unsigned getWindowCount(){ return windows.size(); }
		GUIWindow* at(int index){ if (index < 0 || index >= windows.size()) return nullptr; return windows[index]; }
		void setSystemDepth(uint16_t newDepth);

	private:
		void updateDepths();

		GUIWindow* focusedWindow;
		std::vector<GUIWindow*> windows;
		std::vector<GUIPopup*> popups;
		uint16_t systemDepth;
	};
}