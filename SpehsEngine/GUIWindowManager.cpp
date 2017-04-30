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
	GUIWindowManager::GUIWindowManager(BatchManager& _batchManager) : focusedWindow(nullptr), depthPerWindow(256), batchManager(_batchManager),
		receivingInput(false), mouseHoverAny(false), streching(false), dragging(false)
	{
		batchManager.beginSection();
		popupShade = Polygon::create(Shape::BUTTON, 0, spehs::ApplicationData::getWindowWidth(), spehs::ApplicationData::getWindowHeight());
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
		popup->setPositionGlobal(spehs::ApplicationData::getWindowWidthHalf() - popup->getWidth() * 0.5f, spehs::ApplicationData::getWindowHeightHalf() - popup->getHeight() * 0.5f);
		popups.push_back(popup);
		updateDepths();
		popup->GUIRectangleContainer::inputUpdate();
		popup->GUIRectangleContainer::visualUpdate();
		batchManager.endSection();
	}
	void GUIWindowManager::update()
	{
		batchManager.beginSection();

		const bool focusedWindowReceivingInput = focusedWindow ? focusedWindow->isReceivingInput() : false;
		bool updateWindows = true;
		receivingInput = focusedWindowReceivingInput;
		mouseHoverAny = false;
		streching = false;
		dragging = false;

		if (focusedWindow)
		{
			if (!focusedWindow->getInputEnabled() ||	//Check if focused window yielded focus internally
				(!focusedWindow->getMouseHoverAny() &&	//Focused window is no longer under mouse 
				!focusedWindowReceivingInput))			//Focused window does not receive input
				focusedWindow = nullptr;
			else
			{
				if (focusedWindow->isStreching())
					streching = true;
				if (focusedWindow->isDragging())
					dragging = true;
				if (focusedWindow->getMouseHoverAny())
					mouseHoverAny = true;
			}
		}

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
			{
				updateWindows = false;
				mouseHoverAny = true;
			}
			
			if (popups.front()->checkState(GUIRECT_REMOVE_BIT))
			{//Remove requested
				delete popups.front();
				popups.erase(popups.begin());
				updateDepths();
			}

			//Prevent mouse access outside popups
			inputManager->tryClaimMouseAvailability();
		}
		else if (popupShade->getRenderState())
		{
			popupShade->setRenderState(false);
			popupShade->setColorAlpha(0.0f);
		}

		//Refresh requests before updates: if done after the updates, render would end up displaying un-updated GUI
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i]->refreshRequests > 0 && windows[i]->isOpen())
				windows[i]->refresh();
		}
		
		//Update windows in order
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

					if (windows[i]->getMouseHoverAny() && !focusedWindow)
					{
						focusedWindow = windows[i];
						mouseHoverAny = true;
					}
					else if (windows[i]->isDragging() && !focusedWindow)
					{
						focusedWindow = windows[i];
						dragging = true;
					}
					else if (windows[i]->isStreching() && !focusedWindow)
					{
						focusedWindow = windows[i];
						streching = true;
					}
					else if (windows[i]->isReceivingInput() && !focusedWindow)
					{
						focusedWindow = windows[i];
						receivingInput = true;
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

				if (!focusedWindow->checkState(GUIRECT_MOUSE_HOVER_CONTAINER_BIT) && !focusedWindow->isReceivingInput())
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

		//Claim mouse availability if mouse is hovering over a window
		if (getMouseHoverAny())
			inputManager->tryClaimMouseAvailability();

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