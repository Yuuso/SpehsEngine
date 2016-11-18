#include "ApplicationData.h"
#include "InputManager.h"
#include "GUIWindowManager.h"
#include "GUIWindow.h"
#include "GUIPopup.h"
#include "Console.h"
#include "Polygon.h"
#include "Time.h"

namespace spehs
{
	GUIWindowManager::GUIWindowManager() : focusedWindow(nullptr), depthPerWindow(256)
	{
		popupShade = Polygon::create(Shape::BUTTON, 0, applicationData->getWindowWidth(), applicationData->getWindowHeight());
		popupShade->setCameraMatrixState(false);
		popupShade->setPosition(0, 0);
		setPopupShadeColor(40, 55, 45, 80);

		setSystemDepth(GUIRectangle::defaultDepth);
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
		windows.push_back(window);
		window->setRenderState(false);
		updateDepths();
	}
	void GUIWindowManager::addPopup(GUIPopup* popup)
	{
		popup->setRenderState(true);
		popup->setPositionGlobal(applicationData->getWindowWidthHalf() - popup->getWidth() * 0.5f, applicationData->getWindowHeightHalf() - popup->getHeight() * 0.5f);
		popups.push_back(popup);
		updateDepths();
		popup->GUIRectangleContainer::update();
		popup->GUIRectangleContainer::postUpdate();
	}
	void GUIWindowManager::update()
	{
		bool focusedWindowReceivingInput(false);
		bool updateWindows(true);
		if (focusedWindow)
		{
			if (!focusedWindow->checkState(GUIRECT_ENABLED_BIT))//Check if focused window yielded focus internally
				focusedWindow = nullptr;
			else if (focusedWindow->isReceivingInput())
				focusedWindowReceivingInput = true;
		}

		//Update front popup
		if (!popups.empty())
		{
			//Render state & alpha
			popupShade->setRenderState(true);
			if (popupShade->getColorAlpha() < popupShadeAlpha)
			{
				float a(popupShade->getColorAlpha() + time::getDeltaTime().asSeconds);
				if (a > popupShadeAlpha)
					a = popupShadeAlpha;
				popupShade->setColorAlpha(a);
			}

			//Update
			popups.front()->update();
			popups.front()->postUpdate();
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
		focusedWindow = nullptr;
		if (updateWindows)
		{
			//Update from the top
			for (int i = int(windows.size()) - 1; i >= 0; i--)
			{
				if (windows[i]->isOpen())
				{//Window must be open

					//Run an update on this window, updates mouse hover variable
					windows[i]->update();
					windows[i]->postUpdate();

					if ((windows[i]->checkState(GUIRECT_MOUSE_HOVER_CONTAINER) ||
						windows[i]->checkState(GUIRECT_DRAGGING_BIT) ||
						windows[i]->checkState(GUIRECT_STRECHING_BIT) ||
						windows[i]->isReceivingInput()) && !focusedWindow)
					{//First window to be under focus

						windows[i]->enable();
						focusedWindow = windows[i];

						if (windows[i] != windows.back() &&
							(windows[i]->checkState(GUIRECT_MOUSE_HOVER_CONTAINER) && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT)))
						{//Move window to the back of the vector
							windows.erase(windows.begin() + i);
							windows.push_back(focusedWindow);
							updateDepths();
						}
					}
					else if (windows[i] != focusedWindow)
						windows[i]->disable();
				}
			}

			if (focusedWindow)
			{
				//Yielding focus
				if (inputManager->isKeyPressed(KEYBOARD_ESCAPE) && !focusedWindowReceivingInput)
				{
					focusedWindow = nullptr;
				}
				else if (!focusedWindow->checkState(GUIRECT_MOUSE_HOVER_CONTAINER) && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
				{
					focusedWindow = nullptr;
				}
			}
		}

		//Refresh requested
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i]->checkState(GUIRECT_REFRESH_BIT) && windows[i]->isOpen())
				windows[i]->refresh();
		}
	}
	void GUIWindowManager::refreshWindows()
	{
		for (unsigned i = 0; i < windows.size(); i++)
			windows[i]->refresh();
	}
	void GUIWindowManager::toggleWindow(spehs::GUIWindow* window)
	{
		for (unsigned i = 0; i < windows.size(); i++)
			if (windows[i] == window)
		{
			if (window->isOpen())
				closeWindow(window);
			else
				openWindow(window);
			return;
		}
		spehs::console::warning("Trying to toggle window that is not under window manager!");
	}
	void GUIWindowManager::openWindow(spehs::GUIWindow* window)
	{
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
							return;
						}
					}
				}
				return;
			}
		}
		spehs::console::warning("Trying to open window that is not under window manager!");
	}
	void GUIWindowManager::closeWindow(GUIWindow* window)
	{
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i] == window)
			{
				window->close();
				if (focusedWindow == window)
					focusedWindow = nullptr;
				return;
			}
		}
		spehs::console::warning("Trying to close window that is not under window manager!");
	}
	bool GUIWindowManager::receivingInput()
	{
		if (!focusedWindow)
			return false;
		return focusedWindow->isReceivingInput();
	}
	bool GUIWindowManager::getMouseHoverAny()
	{
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i]->getMouseHoverAny())
				return true;
		}
		return false;
	}
	void GUIWindowManager::setSystemDepth(int16_t depth)
	{
		if (systemDepth != depth)
		{
			systemDepth = depth;
			updateDepths();
		}
	}
	void GUIWindowManager::setDepthPerWindow(int16_t depth)
	{
		if (depthPerWindow != depth)
		{
			depthPerWindow = depth;
			updateDepths();
		}
	}
	void GUIWindowManager::updateDepths()
	{
		for (unsigned i = 0; i < windows.size(); i++)
			windows[i]->setDepth(systemDepth + i * depthPerWindow);
		popupShade->setPlaneDepth(systemDepth + windows.size() * depthPerWindow);
		for (unsigned i = 0; i < popups.size(); i++)
			popups[i]->setDepth(systemDepth + windows.size() * depthPerWindow + 1 + (popups.size() - 1) * 20 - i * 20);
	}
	void GUIWindowManager::setPopupShadeColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		popupShade->setColor(r, g, b, a);
		popupShadeAlpha = a / 255.0f;
	}
}