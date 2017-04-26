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
		GUIWindow* getFocusedWindow() const { return focusedWindow; }
		void toggleWindow(GUIWindow* window);
		void openWindow(GUIWindow* window);
		void closeWindow(GUIWindow* window);//Does not modify window order
		bool isReceivingInput() const { return receivingInput; }
		bool getMouseHoverAny() const { return mouseHoverAny; }
		bool isDragging() const { return dragging; }
		bool isStreching() const{ return streching; }
		unsigned getWindowCount() const { return windows.size(); }
		unsigned getPopupCount() const { return popups.size(); }
		GUIWindow* back() const { return windows.back(); }
		GUIWindow* at(const int index) const { if (index < 0 || index >= windows.size()) return nullptr; return windows[index]; }
		void setSystemDepth(const int16_t depth);
		void setDepthPerWindow(const int16_t depth);
		int16_t getSystemDepth() const { return systemDepth; }
		int16_t getTopDepth() const { return systemDepth + depthPerWindow * windows.size(); }
		int16_t getDepthPerWindow() const { return depthPerWindow; }
		void setPopupShadeColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);

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
		bool receivingInput;
		bool mouseHoverAny;
		bool streching;
		bool dragging;
	};
}