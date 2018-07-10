#pragma once
#include <vector>
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/GUI/GUIContext.h"

namespace spehs
{
	class GUIRectangle;
	class GUIWindow;
	class Polygon;
	class GUIContext;
	class BatchManager;
	class InputManager;

	///Windows are stored in a vector. This vector may change ordering based on performed actions, therefore external for loops and such should be used very carefully!
	class GUIWindowManager : public GUIContext
	{
	public:
		GUIWindowManager(GUIContext& context);//Batch manager where the windows are drawn and where future primitives will be drawn by the window manager
		~GUIWindowManager();

		void update();
		void addWindow(GUIWindow* window);
		void addPopup(GUIRectangle* popup);
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
		GUIWindow* at(const size_t index) const { if (index >= windows.size()) return nullptr; return windows[index]; }
		void setSystemDepth(const int16_t depth);
		void setDepthPerWindow(const int16_t depth);
		int16_t getSystemDepth() const { return systemDepth; }
		int16_t getTopDepth() const { return int16_t(systemDepth + depthPerWindow * windows.size()); }
		int16_t getDepthPerWindow() const { return depthPerWindow; }
		void setPopupShadeColor(const spehs::Color& color);
		
	private:
		void updateDepths();

		GUIWindow* focusedWindow = nullptr;
		std::vector<GUIWindow*> windows;
		std::vector<GUIRectangle*> popups;
		int16_t systemDepth = 0;
		int16_t depthPerWindow = 256;
		Polygon* popupShade = nullptr;
		float popupShadeCurrentAlpha = 0.0f;
		float popupShadeTargetAlpha = 0.0f;
		bool receivingInput = false;
		bool mouseHoverAny = false;
		bool streching = false;
		bool dragging = false;
	};
}