#include "stdafx.h"
#include "SpehsEngine/Input/InputManager.h"

#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Input/EventCatcher.h"
#include "SpehsEngine/Rendering/Window.h"

#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <SDL/SDL_events.h>

#include <iostream>

#define SINT16_MIN -32768
#define SINT16_MAX  32767
#define SINT16_STATES 65536

namespace se
{
	namespace input
	{
		InputManager::InputManager(se::rendering::Window& _window, EventCatcher& _eventCatcher)
			: window(_window)
			, eventCatcher(_eventCatcher)
		{
			SDL_InitSubSystem(SDL_INIT_JOYSTICK);
		}

		InputManager::~InputManager()
		{
			while (!joysticks.empty())
			{
				delete joysticks.back();
				joysticks.pop_back();
			}
		}

		void InputManager::update()
		{
			for (auto& it : keyMap) //foreach loop, c++11
			{
				previousKeyMap[it.first] = it.second;
			}

			if (SDL_NumJoysticks() < joystickCount)
				joystickDisconnected();
			else if (SDL_NumJoysticks() > joystickCount)
				joystickConnected();
			
			std::vector<KeyboardPressEvent>& keyboardPressEvents = eventCatcher.getKeyboardPressEvents();
			std::vector<KeyboardReleaseEvent>& keyboardReleaseEvents = eventCatcher.getKeyboardReleaseEvents();
			std::vector<MouseButtonPressEvent>& mouseButtonPressEvents = eventCatcher.getMouseButtonPressEvents();
			std::vector<MouseButtonReleaseEvent>& mouseButtonReleaseEvents = eventCatcher.getMouseButtonReleaseEvents();
			std::vector<MouseMotionEvent>& mouseMotionEvents = eventCatcher.getMouseMotionEvents();
			std::vector<MouseWheelEvent>& mouseWheelEvents = eventCatcher.getMouseWheelEvents();
			std::vector<MouseHoverEvent>& mouseHoverEvents = eventCatcher.getMouseHoverEvents();
			std::vector<QuitEvent>& quitEvents = eventCatcher.getQuitEvents();
			std::vector<FileDropEvent>& fileDropEvents = eventCatcher.getFileDropEvents();

			mouseAvailable = true;
			mouseWheelDelta = 0;
			mouseMovement = glm::ivec2();
			for (size_t i = 0; i < keyboardPressEvents.size(); i++)
			{
				pressKey(keyboardPressEvents[i].key);
				latestKeyboardPress = keyboardPressEvents[i].key;
			}
			for (size_t i = 0; i < keyboardReleaseEvents.size(); i++)
			{
				releaseKey(keyboardReleaseEvents[i].key);
			}
			for (size_t i = 0; i < mouseButtonPressEvents.size(); i++)
			{
				pressKey(mouseButtonPressEvents[i].button);
				latestKeyboardPress = mouseButtonPressEvents[i].button;
			}
			for (size_t i = 0; i < mouseButtonReleaseEvents.size(); i++)
			{
				releaseKey(mouseButtonReleaseEvents[i].button);
			}
			for (size_t i = 0; i < mouseMotionEvents.size(); i++)
			{
				setMouseCoords(int(mouseMotionEvents[i].position.x), int(mouseMotionEvents[i].position.y));
				mouseMovement = mouseMotionEvents[i].position - mouseMotionEvents[i].previousPosition;
			}
			for (size_t i = 0; i < mouseWheelEvents.size(); i++)
			{
				mouseWheelDelta = mouseWheelEvents[i].wheelDelta;
			}
			for (size_t i = 0; i < quitEvents.size(); i++)
			{
				quitRequested = true;
			}
			for (size_t i = 0; i < fileDropEvents.size(); i++)
			{
				droppedFilePath = fileDropEvents[i].filepath;
			}

			if (mouseLocked)
			{
				//If mouse is locked, keep mouse in the center of the screen without creating a mousemotion event
				SDL_WarpMouseInWindow(window.sdlWindow, window.getWidth() / 2, window.getHeight() / 2);
			}
		}

		void InputManager::pressKey(unsigned int keyID)
		{
			keyMap[keyID] = true;
		}

		void InputManager::releaseKey(unsigned int keyID)
		{
			keyMap[keyID] = false;
		}

		void InputManager::setMouseCoords(int _x, int _y)
		{
			mouseCoords.x = (float)_x;
			mouseCoords.y = (float)_y;
		}

		bool InputManager::lockMouse(const bool _value)
		{
			if (mouseLocked == _value)
				return true;
			else
				mouseLocked = _value;

			if (SDL_SetRelativeMouseMode((SDL_bool)_value) == 0)
			{
				return true;
			}
			else
			{
				SDL_SetWindowGrab(window.sdlWindow, (SDL_bool)_value);
				SDL_ShowCursor(!_value);
				return true;
			}
		}

		bool InputManager::tryClaimMouseAvailability()
		{
			if (!mouseAvailable)
				return false;
			mouseAvailable = false;
			return true;
		}

		bool InputManager::checkMouseAvailability() const
		{
			return mouseAvailable;
		}

		bool InputManager::isKeyDown(unsigned int keyID) const
		{
			auto it = keyMap.find(keyID);
			if (it != keyMap.end())
				return it->second;
			else
				return false;
		}

		bool InputManager::isKeyPressed(unsigned int keyID) const
		{
			if (isKeyDown(keyID) == true && wasKeyDown(keyID) == false)
			{
				return true;
			}
			return false;
		}

		bool InputManager::isKeyReleased(unsigned int keyID) const
		{
			if (isKeyDown(keyID) == false && wasKeyDown(keyID) == true)
			{
				return true;
			}
			return false;
		}

		bool InputManager::wasKeyDown(unsigned int keyID) const
		{
			auto it = previousKeyMap.find(keyID);
			if (it != previousKeyMap.end())
				return it->second;
			else
				return false;
		}

		bool InputManager::fileDropped() const
		{
			if (droppedFilePath.empty())
				return false;
			return true;
		}

		bool InputManager::mouseCollision(const glm::vec4& AABB) const
		{
			return mouseCoords.x >= AABB.x &&
				mouseCoords.x <= AABB.x + AABB.z &&
				mouseCoords.y >= AABB.y &&
				mouseCoords.y <= AABB.y + AABB.w;
		}

		bool InputManager::mouseCollision(const glm::vec2& AABBMin, const glm::vec2& AABBMax) const
		{
			return mouseCoords.x >= AABBMin.x &&
				mouseCoords.x <= AABBMax.x &&
				mouseCoords.y >= AABBMin.y &&
				mouseCoords.y <= AABBMax.y;
		}

		bool InputManager::mouseCollision(const float left, const float right, const float top, const float bottom) const
		{
			return mouseCoords.x >= left &&
				mouseCoords.x <= right &&
				mouseCoords.y >= bottom &&
				mouseCoords.y <= top;
		}

		bool InputManager::isCtrlDown() const
		{
			return isKeyDown(KEYBOARD_LCTRL) || isKeyDown(KEYBOARD_RCTRL);
		}

		bool InputManager::isShiftDown() const
		{
			return isKeyDown(KEYBOARD_LSHIFT) || isKeyDown(KEYBOARD_RSHIFT);
		}

		bool InputManager::isCtrlPressed() const
		{
			return isKeyPressed(KEYBOARD_LCTRL) || isKeyPressed(KEYBOARD_RCTRL);
		}

		bool InputManager::isCtrlReleased() const
		{
			return isKeyReleased(KEYBOARD_LCTRL) || isKeyReleased(KEYBOARD_RCTRL);
		}

		bool InputManager::isShiftPressed() const
		{
			return isKeyPressed(KEYBOARD_LSHIFT) || isKeyPressed(KEYBOARD_RSHIFT);
		}

		bool InputManager::isShiftReleased() const
		{
			return isKeyReleased(KEYBOARD_LSHIFT) || isKeyReleased(KEYBOARD_RSHIFT);
		}



		///////////////////////////
		//  JOYSTICK MANAGEMENT  //
		void InputManager::joystickConnected()
		{//Called when joystick count has increased

			//For every new joystick
			for (int c = 0; c < SDL_NumJoysticks(); c++)
			{
				//Check whether this index is open...
				SDL_Joystick* checkJs = SDL_JoystickOpen(c);
				bool alreadyOpen = false;
				for (unsigned i = 0; i < joysticks.size(); i++)
				{
					if (joysticks[i]->joystick == checkJs)
					{
						alreadyOpen = true;
						break;
					}
				}

				if (!alreadyOpen)
				{//This is an unopened joystick

					//Check suitable offline joysticks
					bool foundOffline = false;
					for (unsigned i = 0; i < joysticks.size(); i++)
						if (joysticks[i]->offline && joysticks[i]->guid == SDL_JoystickGetDeviceGUID(c))
						{//Found matching offline joystick, bring it online
							SDL_Joystick* js = SDL_JoystickOpen(c);
							if (js == NULL)
							{
								std::string error = "Couldn't open SDL joystick! ";
								error += SDL_GetError();
								se::log::error(error);
							}
							joysticks[i]->goOnline(js);
							foundOffline = true;
							break;
						}

					if (!foundOffline)
					{//No matching offline joystick found, create new
						joysticks.push_back(new Joystick(*this, c));
					}
				}
			}

			//Count online joysticks
			joystickCount = 0;
			for (unsigned i = 0; i < joysticks.size(); i++)
				if (!joysticks[i]->offline)
					joystickCount++;
		}

		void InputManager::joystickDisconnected()
		{//Called when joystick count has decreased

			//Check all online joysticks
			for (unsigned i = 0; i < joysticks.size(); i++)
			{
				if (!joysticks[i]->offline)
				{
					if (!SDL_JoystickGetAttached(joysticks[i]->joystick))
						joysticks[i]->goOffline();
				}
			}

			//Count online joysticks
			joystickCount = 0;
			for (unsigned i = 0; i < joysticks.size(); i++)
				if (!joysticks[i]->offline)
					joystickCount++;
		}

		Joystick* InputManager::getJoystick(GUID guid, int preferredIndex)
		{
			Joystick* js = nullptr;
			int index = 0;
			for (unsigned i = 0; i < joysticks.size(); i++)
				if (joysticks[i]->guid == guid)
				{//Found GUID
					if (index == preferredIndex)
						return joysticks[i];
					//Not preferred index, keep looking
					js = joysticks[i];
					index++;
				}

			return js;//Joystick is not connected, or preferred index not found
		}

		int InputManager::getGUIDIndex(int/*NOTE: i < joystickIndex - 1*/ joystickIndex)
		{
			if (joystickIndex >= (int)joysticks.size() || joystickIndex < 0)
				return -1;
			GUID guid = SDL_JoystickGetDeviceGUID(joystickIndex);
			int index = 0;
			for (int i = 0; i < joystickIndex - 1; i++)
			{
				if (joysticks[i]->guid == guid)
					index++;
			}
			return index;
		}
	}
}