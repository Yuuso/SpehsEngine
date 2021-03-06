#include "stdafx.h"

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/GUI/GUIWindowManager.h"
#include "SpehsEngine/GUI/GUIWindow.h"
#include "SpehsEngine/GUI/GUIPopup.h"
#include "SpehsEngine/Rendering/Polygon.h"

namespace se
{
	GUIWindowManager::GUIWindowManager(GUIContext& context)
		: GUIContext(context)
	{
		popupShade = getBatchManager().createPolygon(Shape::BUTTON, 0, float(getBatchManager().window.getWidth()), float(getWindow().getHeight()));
		popupShade->setCameraMatrixState(false);
		popupShade->setPosition(0.0f, 0.0f);
		setPopupShadeColor(se::Color(40, 55, 45, 80));

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
		updateDepths();
	}

	void GUIWindowManager::addPopup(GUIRectangle* popup)
	{
		popup->setRenderState(true);
		popup->setPositionGlobal(getWindow().getWidth() / 2 - popup->getWidth() / 2, getWindow().getHeight() / 2 - popup->getHeight() / 2);
		popups.push_back(popup);
		updateDepths();
		popup->visualUpdate();
	}

	void GUIWindowManager::update()
	{
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
			if (popupShadeCurrentAlpha < popupShadeTargetAlpha)
			{
				popupShadeCurrentAlpha += getDeltaTimeSystem().deltaTime.asSeconds();
				if (popupShadeCurrentAlpha > popupShadeTargetAlpha)
					popupShadeCurrentAlpha = popupShadeTargetAlpha;
				popupShade->setAlpha(popupShadeCurrentAlpha);
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
			getInputManager().tryClaimMouseAvailability();
		}
		else if (popupShade->getRenderState())
		{
			popupShade->setRenderState(false);
			popupShade->setAlpha(0);
			popupShadeCurrentAlpha = 0.0;
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
					{
						windows[i]->disableInput();//Yield enabled input for the next in loop
					}

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
				else if (getInputManager().isKeyPressed(MOUSE_BUTTON_LEFT))
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
					for (size_t i = windows.size(); i-- > 0;)
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
			getInputManager().tryClaimMouseAvailability();
	}

	void GUIWindowManager::refreshWindows()
	{
		for (unsigned i = 0; i < windows.size(); i++)
			windows[i]->refresh();
	}

	void GUIWindowManager::toggleWindow(se::GUIWindow* window)
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
		log::warning("Trying to toggle window that is not under window manager!");
	}

	void GUIWindowManager::openWindow(se::GUIWindow* window)
	{
		for (unsigned i = 0; i < windows.size(); i++)
		{
			if (windows[i] == window)
			{
				window->open();
				if (window != windows.back())
				{//Send window to back of the windows vector
					for (size_t j = 0; j < windows.size(); j++)
					{
						if (windows[j] == window)
						{
							windows.erase(windows.begin() + j);
							windows.push_back(window);
							updateDepths();
							return;
						}
					}
				}
				return;
			}
		}
		log::warning("Trying to open window that is not under window manager!");
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
		log::warning("Trying to close window that is not under window manager!");
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
		for (size_t i = 0; i < windows.size(); i++)
		{
			windows[i]->setDepth(systemDepth + int16_t(i * depthPerWindow));
		}
		popupShade->setPlaneDepth(int16_t(systemDepth + windows.size() * depthPerWindow));
		for (size_t i = 0; i < popups.size(); i++)
		{
			popups[i]->setDepth(int16_t(systemDepth + windows.size() * depthPerWindow + 1 + (popups.size() - 1) * 20 - i * 20));
		}
	}

	void GUIWindowManager::setPopupShadeColor(const se::Color& col)
	{
		popupShade->setColor(col);
		popupShadeTargetAlpha = col.a;
	}
}