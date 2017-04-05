
#include "InputManager.h"
#include "ApplicationData.h"
#include "TextureManager.h"
#include "SpehsEngine.h"
#include "Exceptions.h"
#include "Window.h"

#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <SDL/SDL_events.h>

#include <iostream>

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

	InputManager::InputManager() : mouseCoords(0, 0), mouseMovement(0, 0), droppedFilePath("")
	{
	}
	InputManager::~InputManager()
	{
	}

	void InputManager::initialize()
	{
		SDL_InitSubSystem(SDL_INIT_JOYSTICK);
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

				//Taking a screenshot. NOTE: for some reason, when the printscreen button is pressed, it triggers the key release in the same update cycle, therefore is key down method cannot be used to capture this button's press event!
				if (mEvent.key.keysym.sym == KEYBOARD_PRINTSCREEN)
					textureManager->takeScreenShot();
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

		if (mouseLocked)
		{
			//If mouse is locked, keep mouse in the center of the screen without creating a mousemotion event
			//SDL_WarpMouseInWindow(mainWindow->sdlWindow, applicationData->getWindowWidthHalf(), applicationData->getWindowHeightHalf());
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
		mouseCoords.x = _x;
		mouseCoords.y = _y;
	}

	bool InputManager::lockMouse(const bool _value)
	{
		if (mouseLocked == _value)
			return true;
		else
			mouseLocked = _value;

		SDL_SetWindowGrab(getMainWindow()->sdlWindow, (SDL_bool) _value);
		SDL_ShowCursor(!_value);
		return true;
		//if (SDL_SetRelativeMouseMode((SDL_bool) _value) == 0)
		//{
		//	return true;
		//}
		//else
		//{
		//	exceptions::warning("Mouse locking not supported!");
		//	return false;
		//}
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
						spehs::exceptions::unexpectedError(error);
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
	int InputManager::getGUIDIndex(int/*NOTE: i < joystickIndex - 1*/ joystickIndex)
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
			spehs::exceptions::unexpectedError(error);
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
	unsigned Joystick::getGUIDIndex()
	{
		unsigned index = 0;
		for (unsigned i = 0; i < inputManager->joysticks.size(); i++)
		{
			if (inputManager->joysticks[i] == this)
				return index;
			else if (inputManager->joysticks[i]->guid == guid)
				index++;//Found same guid among joysticks before this
		}
	}
}


std::string getKeyboardKeyAsString(KeyboardKey key)
{
	switch (key)
	{
	case KEYBOARD_UNKNOWN: return "Unknown";
	case KEYBOARD_RETURN: return"Return";
	case KEYBOARD_ESCAPE: return"Escape";
	case KEYBOARD_BACKSPACE: return"KP_3";
	case KEYBOARD_TAB: return"Tab";
	case KEYBOARD_SPACE: return"Space";
	case KEYBOARD_EXCLAIM: return"Exclaim";
	case KEYBOARD_QUOTEDBL: return"\"";
	case KEYBOARD_HASH: return"#";
	case KEYBOARD_PERCENT: return"%";
	case KEYBOARD_DOLLAR: return"$";
	case KEYBOARD_AMPERSAND: return"&";
	case KEYBOARD_QUOTE: return"'";
	case KEYBOARD_LEFTPAREN: return "(";
	case KEYBOARD_RIGHTPAREN: return ")";
	case KEYBOARD_ASTERISK: return "*";
	case KEYBOARD_PLUS: return "+";
	case KEYBOARD_COMMA: return ",";
	case KEYBOARD_MINUS: return "-";
	case KEYBOARD_PERIOD: return ".";
	case KEYBOARD_SLASH: return "/";
	case KEYBOARD_0: return "0";
	case KEYBOARD_1: return "1";
	case KEYBOARD_2: return "2";
	case KEYBOARD_3: return "3";
	case KEYBOARD_4: return "4";
	case KEYBOARD_5: return "5";
	case KEYBOARD_6: return "6";
	case KEYBOARD_7: return "7";
	case KEYBOARD_8: return "8";
	case KEYBOARD_9: return "9";
	case KEYBOARD_COLON: return ":";
	case KEYBOARD_SEMICOLON: return ";";
	case KEYBOARD_LESS: return "<";
	case KEYBOARD_EQUALS: return "=";
	case KEYBOARD_GREATER: return ">";
	case KEYBOARD_QUESTION: return "?";
	case KEYBOARD_AT: return "@";
	case KEYBOARD_LEFTBRACKET: return "{";
	case KEYBOARD_BACKSLASH: return "\\";
	case KEYBOARD_RIGHTBRACKET: return "}";
	case KEYBOARD_CARET: return "^";
	case KEYBOARD_UNDERSCORE: return "_";
	case KEYBOARD_BACKQUOTE: return "`";
	case KEYBOARD_A: return "A";
	case KEYBOARD_B: return "B";
	case KEYBOARD_C: return "C";
	case KEYBOARD_D: return "D";
	case KEYBOARD_E: return "E";
	case KEYBOARD_F: return "F";
	case KEYBOARD_G: return "G";
	case KEYBOARD_H: return "H";
	case KEYBOARD_I: return "I";
	case KEYBOARD_J: return "J";
	case KEYBOARD_K: return "K";
	case KEYBOARD_L: return "L";
	case KEYBOARD_M: return "M";
	case KEYBOARD_N: return "N";
	case KEYBOARD_O: return "O";
	case KEYBOARD_P: return "P";
	case KEYBOARD_Q: return "Q";
	case KEYBOARD_R: return "R";
	case KEYBOARD_S: return "S";
	case KEYBOARD_T: return "T";
	case KEYBOARD_U: return "U";
	case KEYBOARD_V: return "V";
	case KEYBOARD_W: return "W";
	case KEYBOARD_X: return "X";
	case KEYBOARD_Y: return "Y";
	case KEYBOARD_Z: return "Z";
	case KEYBOARD_CAPSLOCK: return "Caps lock";
	case KEYBOARD_F1: return "F1";
	case KEYBOARD_F2: return "F2";
	case KEYBOARD_F3: return "F3";
	case KEYBOARD_F4: return "F4";
	case KEYBOARD_F5: return "F5";
	case KEYBOARD_F6: return "F6";
	case KEYBOARD_F7: return "F7";
	case KEYBOARD_F8: return "F8";
	case KEYBOARD_F9: return "F9";
	case KEYBOARD_F10: return "F10";
	case KEYBOARD_F11: return "F11";
	case KEYBOARD_F12: return "F12";
	case KEYBOARD_PRINTSCREEN: return "Print screen";
	case KEYBOARD_SCROLLLOCK: return "Scroll lock";
	case KEYBOARD_PAUSE: return "Pause";
	case KEYBOARD_INSERT: return "Insert";
	case KEYBOARD_HOME: return "Home";
	case KEYBOARD_PAGEUP: return "Page up";
	case KEYBOARD_DELETE: return "Delete";
	case KEYBOARD_END: return "End";
	case KEYBOARD_PAGEDOWN: return "Page down";
	case KEYBOARD_RIGHT: return "Right arrow";
	case KEYBOARD_LEFT: return "Left arrow";
	case KEYBOARD_DOWN: return "Down arrow";
	case KEYBOARD_UP: return "Up arrow";
	case KEYBOARD_NUMLOCKCLEAR: return "Num lock/clear";
	case KEYBOARD_KP_DIVIDE: return "KP /";
	case KEYBOARD_KP_MULTIPLY: return "KP *";
	case KEYBOARD_KP_MINUS: return "KP -";
	case KEYBOARD_KP_PLUS: return "KP +";
	case KEYBOARD_KP_ENTER: return "KP Enter";
	case KEYBOARD_KP_1: return "KP 1";
	case KEYBOARD_KP_2: return "KP 2";
	case KEYBOARD_KP_3: return "KP 3";
	case KEYBOARD_KP_4: return "KP 4";
	case KEYBOARD_KP_5: return "KP 5";
	case KEYBOARD_KP_6: return "KP 6";
	case KEYBOARD_KP_7: return "KP 7";
	case KEYBOARD_KP_8: return "KP 8";
	case KEYBOARD_KP_9: return "KP 9";
	case KEYBOARD_KP_0: return "KP 0";
	case KEYBOARD_KP_PERIOD: return "KP .";
	case KEYBOARD_APPLICATION: return "Application";
	case KEYBOARD_POWER: return "Power";
	case KEYBOARD_KP_EQUALS: return "KP =";
	case KEYBOARD_F13: return "F13";
	case KEYBOARD_F14: return "F14";
	case KEYBOARD_F15: return "F15";
	case KEYBOARD_F16: return "F16";
	case KEYBOARD_F17: return "F17";
	case KEYBOARD_F18: return "F18";
	case KEYBOARD_F19: return "F19";
	case KEYBOARD_F20: return "F20";
	case KEYBOARD_F21: return "F21";
	case KEYBOARD_F22: return "F22";
	case KEYBOARD_F23: return "F23";
	case KEYBOARD_F24: return "F24";
	case KEYBOARD_EXECUTE: return "Execute";
	case KEYBOARD_HELP: return "Help";
	case KEYBOARD_MENU: return "Menu";
	case KEYBOARD_SELECT: return "Select";
	case KEYBOARD_STOP: return "Stop";
	case KEYBOARD_AGAIN: return "Again";
	case KEYBOARD_UNDO: return "Undo";
	case KEYBOARD_CUT: return "Cut";
	case KEYBOARD_COPY:return "Copy";
	case KEYBOARD_PASTE: return "Paste";
	case KEYBOARD_FIND: return "Find";
	case KEYBOARD_MUTE: return "Mute";
	case KEYBOARD_VOLUMEUP: return "Volume up";
	case KEYBOARD_VOLUMEDOWN: return "Volume down";
	case KEYBOARD_KP_COMMA: return "KP ,";
	case KEYBOARD_KP_EQUALSAS400: return "KP EQUALSAS400";
	case KEYBOARD_ALTERASE: return "Alterbase";
	case KEYBOARD_SYSREQ: return "Sysreq";
	case KEYBOARD_CANCEL: return "Cancel";
	case KEYBOARD_CLEAR: return "Clear";
	case KEYBOARD_PRIOR: return "Prior";
	case KEYBOARD_RETURN2: return "Return2";
	case KEYBOARD_SEPARATOR: return "Separator";
	case KEYBOARD_OUT: return "Out";
	case KEYBOARD_OPER: return "Oper";
	case KEYBOARD_CLEARAGAIN: return "Clear again";
	case KEYBOARD_CRSEL: return "Crsel";
	case KEYBOARD_EXSEL: return "Exsel";
	case KEYBOARD_KP_00: return "KP 00";
	case KEYBOARD_KP_000: return "KP 000";
	case KEYBOARD_THOUSANDSSEPARATOR: return "Thousands separator";
	case KEYBOARD_DECIMALSEPARATOR: return "Decimal separator";
	case KEYBOARD_CURRENCYUNIT: return "Currency unit";
	case KEYBOARD_CURRENCYSUBUNIT: return "Currency sub unit";
	case KEYBOARD_KP_LEFTPAREN: return "KP (";
	case KEYBOARD_KP_RIGHTPAREN: return "KP )";
	case KEYBOARD_KP_LEFTBRACE: return "KP {";
	case KEYBOARD_KP_RIGHTBRACE: return "KP }";
	case KEYBOARD_KP_TAB: return "KP Tab";
	case KEYBOARD_KP_BACKSPACE: return "KP Backspace";
	case KEYBOARD_KP_A: return "KP A";
	case KEYBOARD_KP_B: return "KP B";
	case KEYBOARD_KP_C: return "KP C";
	case KEYBOARD_KP_D: return "KP D";
	case KEYBOARD_KP_E: return "KP E";
	case KEYBOARD_KP_F: return "KP F";
	case KEYBOARD_KP_XOR: return "KP XOR";
	case KEYBOARD_KP_POWER: return "KP POWER";
	case KEYBOARD_KP_PERCENT: return "KP %";
	case KEYBOARD_KP_LESS: return "KP <";
	case KEYBOARD_KP_GREATER: return "KP >";
	case KEYBOARD_KP_AMPERSAND: return "KP &";
	case KEYBOARD_KP_DBLAMPERSAND: return "KP Double ampersand";
	case KEYBOARD_KP_VERTICALBAR: return "KP Vertical bar";
	case KEYBOARD_KP_DBLVERTICALBAR: return "KP Double vertical bar";
	case KEYBOARD_KP_COLON: return "KP :";
	case KEYBOARD_KP_HASH: return "KP #";
	case KEYBOARD_KP_SPACE: return "KP Space";
	case KEYBOARD_KP_AT: return "KP @";
	case KEYBOARD_KP_EXCLAM: return "KP !";
	case KEYBOARD_KP_MEMSTORE: return "KP Mem store";
	case KEYBOARD_KP_MEMRECALL: return "KP Mem recall";
	case KEYBOARD_KP_MEMCLEAR: return "KP Mem clear";
	case KEYBOARD_KP_MEMADD: return "KP Mem add";
	case KEYBOARD_KP_MEMSUBTRACT: return "KP Mem subtract";
	case KEYBOARD_KP_MEMMULTIPLY: return "KP Mem multiply";
	case KEYBOARD_KP_MEMDIVIDE: return "KP Mem divide";
	case KEYBOARD_KP_PLUSMINUS: return "KP Plus minus";
	case KEYBOARD_KP_CLEAR: return "KP Clear";
	case KEYBOARD_KP_CLEARENTRY: return "KP Clear entry";
	case KEYBOARD_KP_BINARY: return "KP Binary";
	case KEYBOARD_KP_OCTAL: return "KP Octal";
	case KEYBOARD_KP_DECIMAL: return "KP Decimal";
	case KEYBOARD_KP_HEXADECIMAL: return "KP Hexadecimal";
	case KEYBOARD_LCTRL: return "Left ctrl";
	case KEYBOARD_LSHIFT: return "Left shift";
	case KEYBOARD_LALT: return "Left alt";
	case KEYBOARD_LGUI: return "Left GUI";
	case KEYBOARD_RCTRL: return "Right control";
	case KEYBOARD_RSHIFT: return "Right shift";
	case KEYBOARD_RALT: return "Right alt";
	case KEYBOARD_RGUI: return "Right GUI";
	case KEYBOARD_MODE: return "Mode";
	case KEYBOARD_AUDIONEXT: return "Audio next";
	case KEYBOARD_AUDIOPREV: return "Audio prev";
	case KEYBOARD_AUDIOSTOP: return "Audio stop";
	case KEYBOARD_AUDIOPLAY: return "Audio play";
	case KEYBOARD_AUDIOMUTE: return "Audio mute";
	case KEYBOARD_MEDIASELECT: return "Media select";
	case KEYBOARD_WWW: return "www";
	case KEYBOARD_MAIL: return "Mail";
	case KEYBOARD_CALCULATOR: return "Calculator";
	case KEYBOARD_COMPUTER: return "Computer";
	case KEYBOARD_AC_SEARCH: return "AC Search";
	case KEYBOARD_AC_HOME: return "AC Home";
	case KEYBOARD_AC_BACK: return "AC Back";
	case KEYBOARD_AC_FORWARD: return "AC Forward";
	case KEYBOARD_AC_STOP: return "AC Stop";
	case KEYBOARD_AC_REFRESH: return "AC Refresh";
	case KEYBOARD_AC_BOOKMARKS: return "AC Bookmarks";
	case KEYBOARD_BRIGHTNESSDOWN: return "Brightness down";
	case KEYBOARD_BRIGHTNESSUP: return "Brightness up";
	case KEYBOARD_DISPLAYSWITCH: return "Display switch";
	case KEYBOARD_KBDILLUMTOGGLE: return "Illum toggle";
	case KEYBOARD_KBDILLUMDOWN: return "Illum down";
	case KEYBOARD_KBDILLUMUP: return "Illum up";
	case KEYBOARD_EJECT: return "Eject";
	case KEYBOARD_SLEEP: return "Sleep";
	default: break;
	}
	return "unknown";
}