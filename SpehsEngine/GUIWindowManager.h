#pragma once
#include <vector>

namespace spehs
{
	class GUIWindow;
	class GUIWindowManager
	{
	public:
		GUIWindowManager();
		~GUIWindowManager();

		void update();
		void addWindow(GUIWindow* window);
		void refreshWindows();
		GUIWindow* getFocusedWindow(){ return focusedWindow; }
		void setFocusedWindow(GUIWindow* window);
		void toggleWindow(GUIWindow* window);
		void openWindow(GUIWindow* window);
		bool receivingInput();
		unsigned getWindowCount(){ return windows.size(); }
		GUIWindow* at(int index){ if (index < 0 || index >= windows.size()) return nullptr; return windows[index]; }

	private:

		GUIWindow* focusedWindow;
		std::vector<GUIWindow*> windows;

	};
}