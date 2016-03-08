#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <SDL/SDL_events.h>
#include <iostream>
#include "Console.h"
#include "InputManager.h"
#include "ApplicationData.h"
#define SINT16_MIN -32768
#define SINT16_MAX  32767
#define SINT16_STATES 65536


spehs::InputManager* inputManager;
namespace spehs
{
	spehs::GUID getJoystickDeviceGUID(int _deviceIndex)
	{
		return SDL_JoystickGetDeviceGUID(_deviceIndex);
	}

	InputManager::InputManager() : mouseCoords(0), mouseMovement(0, 0), droppedFilePath("")
	{
	}
	InputManager::~InputManager()
	{
	}

	void InputManager::initialize()
	{
		SDL_InitSubSystem(SDL_INIT_JOYSTICK);

		//DEBUG JOYSTICK TESTING
		//joystickConnected();
	}
	void InputManager::uninitialize()
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
		processEvents();

		/*
		//JS DEBUG
		SDL_JoystickUpdate();
		for (unsigned j = 0; j < joysticks.size(); j++)
			if (!joysticks[j]->offline)
		{
			//Buttons
			for (int i = 0; i < joysticks[j]->buttonCount; i++)
			{
				if (joysticks[j]->isButtonDown(i))
					std::cout << "\n[" << j << "]Button " << i << " is down";
			}
			//Axes
			//std::cout << "\n[" << j << "] Axes: ";
			for (int i = 0; i < joysticks[j]->axisCount; i++)
			{
				//std::cout << " [" << i << "]" << joysticks[j]->getAxisState(i);
			}
			//Hats
			for (int i = 0; i < joysticks[j]->hatCount; i++)
			{
				for (int b = 1; b < 9; b++)
					if (joysticks[j]->isHatInPosition(i, b) == SINT16_MAX)
						std::cout << "\nHat" << i << ": " << b << " pressed";
			}
		}*/
	}
	void InputManager::processEvents()
	{
		if (SDL_NumJoysticks() < joystickCount)
			joystickDisconnected();
		else if (SDL_NumJoysticks() > joystickCount)
			joystickConnected();

		mouseWheelDelta = 0;
		static glm::ivec2 prevMouseCoords;
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
				break;
			case SDL_KEYUP:
				releaseKey(mEvent.key.keysym.sym);
				break;
			case SDL_MOUSEMOTION:
				setMouseCoords(int(mEvent.motion.x), int(applicationData->getWindowHeight() - mEvent.motion.y));
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
		mouseCoords.x = _x;
		mouseCoords.y = _y;
	}


	bool InputManager::isKeyDown(unsigned int keyID)
	{
		auto it = keyMap.find(keyID);
		if (it != keyMap.end())
			return it->second;
		else
			return false;
	}


	bool InputManager::isKeyPressed(unsigned int keyID)
	{
		if (isKeyDown(keyID) == true && wasKeyDown(keyID) == false)
		{
			return true;
		}
		return false;
	}


	bool InputManager::wasKeyDown(unsigned int keyID)
	{
		auto it = previousKeyMap.find(keyID);
		if (it != previousKeyMap.end())
			return it->second;
		else
			return false;
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
						spehs::console::error(error);
					}
					joysticks[i]->goOnline(js);
					foundOffline = true;
					break;
					}

				if (!foundOffline)
				{//No matching offline joystick found, create new
					joysticks.push_back(new Joystick(c));
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
	int InputManager::getGUIDIndex(int joystickIndex)
	{
		if (joystickIndex >= joysticks.size() || joystickIndex < 0)
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


	//JOYSTICK STRUCT
	Joystick::~Joystick()
	{
		goOffline();
	}
	Joystick::Joystick(int index)
	{
		joystick = SDL_JoystickOpen(index);
		if (joystick == NULL)
		{
			std::string error = "Failed to open SDL_joystick! ";
			error += SDL_GetError();
			spehs::console::error(error);
			return;
		}

		ID = SDL_JoystickInstanceID(joystick);
		name = SDL_JoystickNameForIndex(index);
		guid = SDL_JoystickGetDeviceGUID(index);
		buttonCount = SDL_JoystickNumButtons(joystick);
		axisCount = SDL_JoystickNumAxes(joystick);
		hatCount = SDL_JoystickNumHats(joystick);
		offline = false;
		std::cout << "\nJoystick created: " << SDL_JoystickName(joystick);
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
	int Joystick::getGUIDIndex()
	{
		int index = 0;
		for (unsigned i = 0; i < inputManager->joysticks.size(); i++)
		{
			if (inputManager->joysticks[i] == this)
				return index;
			else if (inputManager->joysticks[i]->guid == guid)
				index++;//Found similar index among joysticks
		}
	}
}