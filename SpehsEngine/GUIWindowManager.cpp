#include "InputManager.h"
#include "GUIWindowManager.h"
#include "GUIWindow.h"
#include "Console.h"

namespace spehs
{
	GUIWindowManager::GUIWindowManager() : focusedWindow(nullptr)
	{
		setSystemDepth(GUIRectangle::defaultDepth);
	}
	GUIWindowManager::~GUIWindowManager()
	{
		for (unsigned i = 0; i < windows.size(); i++)
			delete windows[i];
	}
	void GUIWindowManager::addWindow(GUIWindow* window)
	{
		windows.push_back(window);
		window->setRenderState(false);
		updateDepths();
	}
	void GUIWindowManager::update()
	{
		bool focusedWindowReceivingInput = false;
		if (focusedWindow)
		{
			if (!focusedWindow->isFocused())//Check if focused window yielded focus internally
				focusedWindow = nullptr;
			else if (focusedWindow->isReceivingInput())
				focusedWindowReceivingInput = true;
		}

		//Search for a window to update from the top
		for (int i = int(windows.size()) - 1; i >= 0; i--)
			if (windows[i]->isOpen())
			{//Window must be open

				//Run an update on this window, updates mouse hover variable
				windows[i]->update();
				windows[i]->postUpdate();

				if (windows[i]->checkState(GUIRECT_MOUSE_HOVER_CONTAINER))
				{//If mouse is hovering over this window, do not update windows below

					if (windows[i]->isFocused() && focusedWindow != windows[i])
					{//Window gained focus internally during mouse activity update

						//Previous active window loses focus
						if (focusedWindow)
							focusedWindow->loseFocus();

						//Update focused window pointer
						focusedWindow = windows[i];

						//Move window to the back of the vector
						if (focusedWindow != windows.back())
						{
							windows.erase(windows.begin() + i);
							windows.push_back(focusedWindow);
							updateDepths();
						}						
					}

					//Break, do not run mouse activity updates in other windows
					break;
				}
			}

		if (focusedWindow)
		{
			//Yielding focus
			if (inputManager->isKeyPressed(KEYBOARD_ESCAPE) && !focusedWindowReceivingInput)
			{
				focusedWindow->loseFocus();
				focusedWindow = nullptr;
			}
			else if (!focusedWindow->checkState(GUIRECT_MOUSE_HOVER_CONTAINER) && inputManager->isKeyPressed(MOUSE_BUTTON_LEFT))
			{
				focusedWindow->loseFocus();
				focusedWindow = nullptr;
			}
		}

	}
	void GUIWindowManager::setFocusedWindow(GUIWindow* window)
	{
		if (focusedWindow)
			focusedWindow->loseFocus();
		focusedWindow = window;
		window->gainFocus();
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
				setFocusedWindow(window);
				if (focusedWindow != windows.back())
				{//Send window to back of the windows vector
					for (unsigned i = 0; i < windows.size(); i++)
					{
						if (windows[i] == focusedWindow)
						{
							windows.erase(windows.begin() + i);
							windows.push_back(focusedWindow);
							updateDepths();
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
	void GUIWindowManager::setSystemDepth(uint16_t newDepth)
	{
		systemDepth = newDepth;
		updateDepths();
	}
	void GUIWindowManager::updateDepths()
	{
		for (unsigned i = 0; i < windows.size(); i++)
			windows[i]->setDepth(systemDepth + i * 256);
	}
}