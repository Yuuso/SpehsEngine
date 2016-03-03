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
		void render();

		void addWindow(GUIWindow* window){ windows.push_back(window); }
		void refreshWindows();
		GUIWindow* getFocusedWindow(){ return focusedWindow; }
		void setFocusedWindow(GUIWindow* window);
		void toggleWindow(GUIWindow* window);
		void openWindow(GUIWindow* window);
		bool receivingInput();

	private:

		GUIWindow* focusedWindow;
		std::vector<GUIWindow*> windows;

	};
}