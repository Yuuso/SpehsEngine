#include "SpehsEngine/Input/InputManager.h"
//#include "SpehsEngine/Rendering/TextureManager.h" TODO!!!
#include "SpehsEngine/Input/input.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Rendering/Window.h"

#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <SDL/SDL_events.h>

#include <iostream>

#define SINT16_MIN -32768
#define SINT16_MAX  32767
#define SINT16_STATES 65536


//se::InputManager* inputManager;
namespace se
{
	se::GUID getJoystickDeviceGUID(int _deviceIndex)
	{
		return SDL_JoystickGetDeviceGUID(_deviceIndex);
	}

	InputManager::InputManager(Window& _window)
		: window(_window)
		, mouseCoords(0, 0)
		, mouseMovement(0, 0)
		, droppedFilePath("")
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
		mouseAvailable = true;
		for (auto& it : keyMap) //foreach loop, c++11
		{
			previousKeyMap[it.first] = it.second;
		}
		processEvents();
	}

	void InputManager::processEvents()
	{
		if (SDL_NumJoysticks() < joystickCount)
			joystickDisconnected();
		else if (SDL_NumJoysticks() > joystickCount)
			joystickConnected();

		mouseWheelDelta = 0;
		static se::ivec2 prevMouseCoords;
		prevMouseCoords = mouseCoords;
		latestKeyboardPress = 0;
		latestMouseButtonPress = 0;

		static SDL_Event mEvent;
		while (SDL_PollEvent(&mEvent))
		{
			switch (mEvent.type)
			{
			case SDL_KEYDOWN:
				pressKey(mEvent.key.keysym.sym);
				latestKeyboardPress = mEvent.key.keysym.sym;

				//Taking a screenshot. NOTE: for some reason, when the printscreen button is pressed, it triggers the key release in the same update cycle, therefore is key down method cannot be used to capture this button's press event!
				//if (mEvent.key.keysym.sym == KEYBOARD_PRINTSCREEN)
					//textureManager->takeScreenShot(); TODO!!!
				break;
			case SDL_KEYUP:
				releaseKey(mEvent.key.keysym.sym);
				break;
			case SDL_MOUSEMOTION:
				setMouseCoords(int(mEvent.motion.x), int(window.getHeight() - mEvent.motion.y));
				break;
			case SDL_MOUSEBUTTONDOWN:
				pressKey(mEvent.button.button);
				latestMouseButtonPress = mEvent.button.button;
				break;
			case SDL_MOUSEBUTTONUP:
				releaseKey(mEvent.button.button);
				break;
			case SDL_MOUSEWHEEL:
				mouseWheelDelta = mEvent.wheel.y;
				break;
			case SDL_QUIT:
				quitRequested = true;
				break;
			case SDL_DROPFILE:
				std::cout << "\n\tFile dropped: " << mEvent.drop.file;
				droppedFilePath = mEvent.drop.file;
				break;
			}
		}

		//Update mouse movement
		mouseMovement = mouseCoords - prevMouseCoords;

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

		if (SDL_SetRelativeMouseMode((SDL_bool) _value) == 0)
		{
			return true;
		}
		else
		{
			SDL_SetWindowGrab(window.sdlWindow, (SDL_bool) _value);
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

	bool InputManager::mouseCollision(const se::vec4& AABB) const
	{
		return mouseCoords.x >= AABB.x &&
			mouseCoords.x <= AABB.x + AABB.z &&
			mouseCoords.y >= AABB.y &&
			mouseCoords.y <= AABB.y + AABB.w;
	}

	bool InputManager::mouseCollision(const se::vec2& AABBMin, const se::vec2& AABBMax) const
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

	bool InputManager::isShiftPressed() const
	{
		return isKeyPressed(KEYBOARD_LSHIFT) || isKeyPressed(KEYBOARD_RSHIFT);
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



	//JOYSTICK
	Joystick::Joystick(InputManager& _inputManager, int index)
		: inputManager(_inputManager)
	{
		joystick = SDL_JoystickOpen(index);
		if (joystick == NULL)
		{
			std::string error = "Failed to open SDL_joystick! ";
			error += SDL_GetError();
			se::log::error(error);
			return;
		}

		ID = SDL_JoystickInstanceID(joystick);
		name = SDL_JoystickNameForIndex(index);
		guid = SDL_JoystickGetDeviceGUID(index);
		buttonCount = SDL_JoystickNumButtons(joystick);
		axisCount = SDL_JoystickNumAxes(joystick);
		hatCount = SDL_JoystickNumHats(joystick);
		offline = false;
		std::string str("Joystick created: ");
		str += SDL_JoystickName(joystick);
		se::log::info(str);
	}

	Joystick::~Joystick()
	{
		goOffline();
	}

	void Joystick::goOffline()
	{
		if (joystick != NULL)
			SDL_JoystickClose(joystick);
		joystick = nullptr;
		offline = true;
		std::cout << "\n" << name << " went offline!";
	}

	void Joystick::goOnline(SDL_Joystick* newJs)
	{
		joystick = newJs;
		offline = false;

		if (!SDL_JoystickGetAttached(newJs))
			std::cout << "\n" << name << " couldn't go online!";
		else
			std::cout << "\n" << name << " went online!";
	}

	float Joystick::getAxisState(int axisIndex)
	{
		if (offline)
			return 0.0f;
		return float(SDL_JoystickGetAxis(joystick, axisIndex))/SINT16_MAX;
	}

	bool Joystick::isButtonDown(int buttonIndex)
	{
		if (offline)
			return false;
		if (SDL_JoystickGetButton(joystick, buttonIndex))
			return true;
		return false;
	}

	bool Joystick::isHatInPosition(int hatIndex, uint8_t position)
	{
		if (offline)
			return false;
		if (SDL_JoystickGetHat(joystick, hatIndex) == position)
			return true;
		return false;
	}

	int32_t Joystick::queryButtonDown()
	{
		if (offline)
			return -1;
		for (int i = 0; i < buttonCount; i++)
		{
			if (SDL_JoystickGetButton(joystick, i))
				return i;
		}
		return -1;
	}

	unsigned Joystick::getGUIDIndex()
	{
		unsigned index = 0;
		for (unsigned i = 0; i < inputManager.joysticks.size(); i++)
		{
			if (inputManager.joysticks[i] == this)
				return index;
			else if (inputManager.joysticks[i]->guid == guid)
				index++;//Found same guid among joysticks before this
		}
		return ~0;
	}
}