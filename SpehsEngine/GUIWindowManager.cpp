#include "ApplicationData.h"
#include "InputManager.h"
#include "BatchManager.h"
#include "GUIWindowManager.h"
#include "Exceptions.h"
#include "GUIWindow.h"
#include "GUIPopup.h"
#include "Polygon.h"
#include "Time.h"

namespace spehs
{
	GUIWindowManager::GUIWindowManager(BatchManager& _batchManager) : focusedWindow(nullptr), depthPerWindow(256), batchManager(_batchManager)
	{
		batchManager.beginSection();
		popupShade = Polygon::create(Shape::BUTTON, 0, applicationData->getWindowWidth(), applicationData->getWindowHeight());
		popupShade->setCameraMatrixState(false);
		popupShade->setPosition(0, 0);
		setPopupShadeColor(40, 55, 45, 80);

		setSystemDepth(GUIRectangle::defaultDepth);
		batchManager.endSection();
	}
	GUIWindowManager::~GUIWindowManager()
	{
		for (unsigned i = 0; i < windows.size(); i++)
			delete windows[i];
		for (unsigned i = 0; i < popups.size(); i++)
			delete popups[i];
		popupShade->destroy();
	}
	void GUIWindowManager::addWindow(GUIWindow* window)
	{
		batchManager.beginSection();
		windows.push_back(window);
		updateDepths();
		batchManager.endSection();
	}
	void GUIWindowManager::addPopup(GUIPopup* popup)
	{
		batchManager.beginSection();
		popup->setRenderState(true);
		popup->setPositionGlobal(applicationData->getWindowWidthHalf() - popup->getWidth() * 0.5f, applicationData->getWindowHeightHalf() - popup->getHeight() * 0.5f);
		popups.push_back(popup);
		updateDepths();
		popup->GUIRectangleContainer::inputUpdate();
		popup->GUIRectangleContainer::visualUpdate();
		batchManager.endSection();
	}
	void GUIWindowManager::update()
	{
		batchManager.beginSection();

		if (focusedWindow)
		{
			if (!focusedWindow->checkState(GUIRECT_INPUT_ENABLED_BIT) ||	//Check if focused window yielded focus internally
				(!focusedWindow->getMouseHoverAny() &&						//Focused window is no longer under mouse 
				!focusedWindow->isReceivingInput()))						//Focused window does not receive input
				focusedWindow = nullptr;
		}

		const bool focusedWindowReceivingInput(focusedWindow ? focusedWindow->isReceivingInput() : false);
		bool updateWindows(true);

		//Update front popup
		if (!popups.empty())
		{
			//Render state & alpha
			popupShade->setRenderState(true);
			if (popupShade->getColorAlpha() < popupShadeAlpha)
			{
				float a(popupShade->getColorAlpha() + time::getDeltaTimeAsSeconds());
				if (a > popupShadeAlpha)
					a = popupShadeAlpha;
				popupShade->setColorAlpha(a);
			}

			//Update
			popups.front()->inputUpdate();
			popups.front()->visualUpdate();
			if (popups.front()->getMouseHoverAny())
				updateWindows = false;
			
			if (popups.front()->checkState(GUIRECT_REMOVE_BIT))
			{//Remove requested
				delete popups.front();
				popups.erase(popups.begin());
				updateDepths();
			}
		}
		else if (popupShade->getRenderState())
		{
			popupShade->setRenderState(false);
			popupShade->setColorAlpha(0.0f);
		}
		
		//Update windows in order
		//focusedWindow = nullptr;
		bool focusWindowUpdated(false);
		if (updateWindows)
		{
			//Update from the top
			for (int i = int(windows.size()) - 1; i >= 0; i--)
			{
				if (windows[i]->isOpen())
				{//Window must be open

					if (!focusedWindowReceivingInput &&
						(!focusWindowUpdated || !focusedWindow))
						windows[i]->enableInput();//Valid for input
					windows[i]->inputUpdate();
					windows[i]->visualUpdate();

					if ((windows[i]->checkState(GUIRECT_MOUSE_HOVER_CONTAINER) ||
						windows[i]->checkState(GUIRECT_DRAGGING_BIT) ||
						windows[i]->checkState(GUIRECT_STRECHING_BIT) ||
						windows[i]->isReceivingInput()) && !focusedWindow)
					{//First window to be under focus
						focusedWindow = windows[i];
					}
					else if (windows[i] != focusedWindow)
						windows[i]->disableInput();//Yield enabled input for the next in loop

					//Mark focused window updated
					if (windows[i] == focusedWindow)
						focusWindowUpdated = true;
				}
			}

			if (focusedWindow)
			{//Yielding focus

				if (!focusedWindow->checkState(GUIRECT_MOUSE_HOVER_CONTAINER))
				{//Window not under mouse, do not continue focus status
					focusedWindow = nullptr;
				}
				else if (inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
				{//Focused window was pressed, move window to the back of the vector if not there already
					if (focusedWindow != windows.back())
					{
						for (unsigned i = 0; i < windows.size(); i++)
						{
							if (windows[i] == focusedWindow)
							{
								windows.erase(windows.begin() + i);
								windows.push_back(focusedWindow);
								updateDepths();
								break;
							}
						}
					}
				}
				else
				{//Check if there are windows on top of focused window -> yield focus
					for (int i = windows.size() - 1; i >= 0; i--)
					{
						if (windows[i] == focusedWindow)
							break;
						else if (windows[i]->checkState(GUIRECT_OPEN_BIT) && windows[i]->getMouseHoverAny())
						{
							focusedWindow = nullptr;
							break;
						}
					}
				}
			}
		}

		//Refresh requested
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i]->checkState(GUIRECT_REFRESH_BIT) && windows[i]->isOpen())
				windows[i]->refresh();
		}
		batchManager.endSection();
	}
	void GUIWindowManager::refreshWindows()
	{
		batchManager.beginSection();
		for (unsigned i = 0; i < windows.size(); i++)
			windows[i]->refresh();
		batchManager.endSection();
	}
	void GUIWindowManager::toggleWindow(spehs::GUIWindow* window)
	{
		batchManager.beginSection();
		for (unsigned i = 0; i < windows.size(); i++)
			if (windows[i] == window)
		{
			if (window->isOpen())
				closeWindow(window);
			else
				openWindow(window);
			batchManager.endSection();
			return;
		}
		spehs::exceptions::warning("Trying to toggle window that is not under window manager!");
		batchManager.endSection();
	}
	void GUIWindowManager::openWindow(spehs::GUIWindow* window)
	{
		batchManager.beginSection();
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i] == window)
			{
				window->open();
				if (window != windows.back())
				{//Send window to back of the windows vector
					for (unsigned i = 0; i < windows.size(); i++)
					{
						if (windows[i] == window)
						{
							windows.erase(windows.begin() + i);
							windows.push_back(window);
							updateDepths();
							batchManager.endSection();
							return;
						}
					}
				}
				batchManager.endSection();
				return;
			}
		}
		spehs::exceptions::warning("Trying to open window that is not under window manager!");
		batchManager.endSection();
	}
	void GUIWindowManager::closeWindow(GUIWindow* window)
	{
		batchManager.beginSection();
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i] == window)
			{
				window->close();
				if (focusedWindow == window)
					focusedWindow = nullptr;
				batchManager.endSection();
				return;
			}
		}
		spehs::exceptions::warning("Trying to close window that is not under window manager!");
		batchManager.endSection();
	}
	bool GUIWindowManager::isReceivingInput() const
	{
		if (!focusedWindow)
			return false;
		batchManager.beginSection();
		bool value(focusedWindow->isReceivingInput());
		batchManager.endSection();
		return value;
	}
	bool GUIWindowManager::getMouseHoverAny() const
	{
		batchManager.beginSection();
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i]->getMouseHoverAny())
			{
				batchManager.endSection();
				return true;
			}
		}
		batchManager.endSection();
		return false;
	}
	void GUIWindowManager::setSystemDepth(const int16_t depth)
	{
		if (systemDepth != depth)
		{
			systemDepth = depth;
			updateDepths();
		}
	}
	void GUIWindowManager::setDepthPerWindow(const int16_t depth)
	{
		if (depthPerWindow != depth)
		{
			depthPerWindow = depth;
			updateDepths();
		}
	}
	void GUIWindowManager::updateDepths()
	{
		batchManager.beginSection();
		for (unsigned i = 0; i < windows.size(); i++)
			windows[i]->setDepth(systemDepth + i * depthPerWindow);
		popupShade->setPlaneDepth(systemDepth + windows.size() * depthPerWindow);
		for (unsigned i = 0; i < popups.size(); i++)
			popups[i]->setDepth(systemDepth + windows.size() * depthPerWindow + 1 + (popups.size() - 1) * 20 - i * 20);
		batchManager.endSection();
	}
	void GUIWindowManager::setPopupShadeColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
	{
		popupShade->setColor(r, g, b, a);
		popupShadeAlpha = a / 255.0f;
	}
}