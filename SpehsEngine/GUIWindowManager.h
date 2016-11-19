#pragma once
#include <vector>

namespace spehs
{
	class GUIPopup;
	class GUIWindow;
	class Polygon;
	class BatchManager;
	///Windows are stored in a vector. This vector may change ordering based on performed actions, therefore external for loops and such should be used very carefully!
	class GUIWindowManager
	{
	public:
		GUIWindowManager(BatchManager& _batchManager);//Batch manager where the windows are drawn and where future primitives will be drawn by the window manager
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
		void setSystemDepth(int16_t depth);
		void setDepthPerWindow(int16_t depth);
		int16_t getSystemDepth(){ return systemDepth; }
		int16_t getDepthPerWindow(){ return depthPerWindow; }
		void setPopupShadeColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

	private:
		void updateDepths();

		GUIWindow* focusedWindow;
		std::vector<GUIWindow*> windows;
		std::vector<GUIPopup*> popups;
		int16_t systemDepth;
		int16_t depthPerWindow;
		Polygon* popupShade;
		float popupShadeAlpha;
		BatchManager& batchManager;
	};
}