#pragma once

#include <unordered_map>

#include <glm/vec2.hpp>

#include "GUID.h"

#define SINT16_MIN -32768
#define SINT16_MAX  32767
#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT 3
#define MOUSE_BUTTON_4 4
#define MOUSE_BUTTON_5 5


struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;
typedef int32_t SDL_JoystickID;
namespace spehs
{
	GUID getJoystickDeviceGUID(int _deviceIndex);
	/**Joysticks can only be created during run time, they will not be removed but instead go into offline mode. \n
	This is because parts of the program can be attached to a joystick through a pointer/reference and unknowingly could try to access the removed joystick's state*/
	class Joystick
	{
	public:
		/*Returns the axis state, range [-1, 1] NOTE: float precision...*/
		float getAxisState(int axisIndex);
		bool isButtonDown(int buttonIndex);
		bool isHatInPosition(int hatIndex, uint8_t position);
		int32_t queryButtonDown();///<Returns -1 if no button is pressed. Returns button index (>= 0) if a button is pressed. If multiple buttons are being pressed, returns the one with the lowerst index.
		unsigned getGUIDIndex();///<Searches for joysticks with the same GUID. Returns my index among devices with the same GUID

		GUID guid;
		SDL_Joystick* joystick;
		SDL_JoystickID ID;
		std::string name;

		int axisCount;
		int buttonCount;
		int hatCount;

		void goOffline();
		void goOnline(SDL_Joystick* newJs);
		bool offline;///<When joystick disconnects during runtime it goes into offline mode. In offline mode key pressed calls return false, axis states return 0.
	private:
		Joystick(int index);
		~Joystick();
		friend class InputManager;
	};


	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void initialize();
		void uninitialize();
		void update();
		void processEvents();

		//manging keyboard
		void pressKey(unsigned int keyID);
		void releaseKey(unsigned int keyID);
		bool isKeyDown(unsigned int keyID) const;
		bool isKeyPressed(unsigned int keyID) const;

		//Special input query
		bool isCtrlDown() const;///< Returns true if left or right ctrl button is down
		bool isShiftDown() const;///< Returns true if left or right ctrl button is down
		bool isCtrlPressed() const;///< Returns true if left or right ctrl button is pressed
		bool isShiftPressed() const;///< Returns true if left or right ctrl button is pressed

		//Managing mouse
		void setMouseCoords(int _x, int _y);
		bool lockMouse(const bool _value);
		bool tryClaimMouseAvailability();
		bool checkMouseAvailability() const;

		//Managing joysticks
		Joystick* getJoystick(GUID guid, int preferredIndex = 0);
		int getGUIDIndex(int joystickIndex);///<Searches for joysticks with the same GUID as given joystick. Returns index of given joystick among devices with the same GUID. Returns -1 on failure

		//Getters
		glm::vec2 getMouseCoords() const { return mouseCoords; }
		glm::vec2 getMouseMovement() const { return mouseMovement; }
		float getMouseX() const { return mouseCoords.x; }
		float getMouseY() const { return mouseCoords.y; }
		float getMouseMovementX() const { return mouseMovement.x; }
		float getMouseMovementY() const { return mouseMovement.y; }
		int getMouseWheelDelta() const { return mouseWheelDelta; }
		bool mouseCollision(const glm::vec4& AABB_leftX_bottomY_width_height) const;
		bool mouseCollision(const glm::vec2& minAABB, const glm::vec2& maxAABB) const;
		bool mouseCollision(const float leftAABB, const float rightAABB, const float topAABB, const float bottomAABB) const;
		bool isQuitRequested() const { return quitRequested; }
		bool fileDropped() const;
		std::string getDroppedFilePath(){ std::string path = droppedFilePath; droppedFilePath.clear();/*Reset dropped file path*/ return path; }

		//Public access members
		std::vector<Joystick*> joysticks;
		int32_t latestKeyboardPress;///< Latest key pressed. Reset for each update (0) if nothing was pressed during that update
		int32_t latestMouseButtonPress;///< Latest mouse buton pressed. Reset for each update (0) if nothing was pressed during that update
		std::unordered_map<unsigned int, bool> keyMap;
		std::unordered_map<unsigned int, bool> previousKeyMap;

	private:
		bool quitRequested = false;
		std::string droppedFilePath;///<Dropped file path is retrievable until any source retrieves(getDroppedFilePath()) it once.
		
		//Keyboard
		bool wasKeyDown(unsigned int keyID) const;

		//Mouse
		bool mouseLocked = false;
		bool mouseAvailable = false;//This boolean indicates global mouse availability during an update cycle. Update cycles should run according to depth from top to bottom. The first source to claim the mouse availability can do so.
		int mouseWheelDelta = 0;
		glm::ivec2 mouseCoords;
		glm::ivec2 mouseMovement;
		
		//Joystick(s)
		void joystickConnected();
		void joystickDisconnected();
		void createJoystick(int index);
		void deleteJoystick(Joystick* ptr);
		int joystickCount = 0;
	};
}
extern spehs::InputManager* inputManager;

/*
SDL Keycodes wrapped to Engines own.
SpehsEngine namespace not used to make using these more convenient.
*/

//KEYBOARD
enum KeyboardKey : unsigned int
{
	KEYBOARD_UNKNOWN = 0x00,

	KEYBOARD_RETURN = '\r',
	KEYBOARD_ESCAPE = '\033',
	KEYBOARD_BACKSPACE = '\b',
	KEYBOARD_TAB = '\t',
	KEYBOARD_SPACE = ' ',
	KEYBOARD_EXCLAIM = '!',
	KEYBOARD_QUOTEDBL = '"',
	KEYBOARD_HASH = '#',
	KEYBOARD_PERCENT = '%',
	KEYBOARD_DOLLAR = '$',
	KEYBOARD_AMPERSAND = '&',
	KEYBOARD_QUOTE = '\'',
	KEYBOARD_LEFTPAREN = '(',
	KEYBOARD_RIGHTPAREN = ')',
	KEYBOARD_ASTERISK = '*',
	KEYBOARD_PLUS = '+',
	KEYBOARD_COMMA = ',',
	KEYBOARD_MINUS = '-',
	KEYBOARD_PERIOD = '.',
	KEYBOARD_SLASH = '/',
	KEYBOARD_0 = '0',
	KEYBOARD_1 = '1',
	KEYBOARD_2 = '2',
	KEYBOARD_3 = '3',
	KEYBOARD_4 = '4',
	KEYBOARD_5 = '5',
	KEYBOARD_6 = '6',
	KEYBOARD_7 = '7',
	KEYBOARD_8 = '8',
	KEYBOARD_9 = '9',
	KEYBOARD_COLON = ':',
	KEYBOARD_SEMICOLON = ';',
	KEYBOARD_LESS = '<',
	KEYBOARD_EQUALS = '=',
	KEYBOARD_GREATER = '>',
	KEYBOARD_QUESTION = '?',
	KEYBOARD_AT = '@',

	KEYBOARD_LEFTBRACKET = '[',
	KEYBOARD_BACKSLASH = '\\',
	KEYBOARD_RIGHTBRACKET = ']',
	KEYBOARD_CARET = '^',
	KEYBOARD_UNDERSCORE = '_',
	KEYBOARD_BACKQUOTE = '`',
	KEYBOARD_A = 'a',
	KEYBOARD_B = 'b',
	KEYBOARD_C = 'c',
	KEYBOARD_D = 'd',
	KEYBOARD_E = 'e',
	KEYBOARD_F = 'f',
	KEYBOARD_G = 'g',
	KEYBOARD_H = 'h',
	KEYBOARD_I = 'i',
	KEYBOARD_J = 'j',
	KEYBOARD_K = 'k',
	KEYBOARD_L = 'l',
	KEYBOARD_M = 'm',
	KEYBOARD_N = 'n',
	KEYBOARD_O = 'o',
	KEYBOARD_P = 'p',
	KEYBOARD_Q = 'q',
	KEYBOARD_R = 'r',
	KEYBOARD_S = 's',
	KEYBOARD_T = 't',
	KEYBOARD_U = 'u',
	KEYBOARD_V = 'v',
	KEYBOARD_W = 'w',
	KEYBOARD_X = 'x',
	KEYBOARD_Y = 'y',
	KEYBOARD_Z = 'z',

	KEYBOARD_CAPSLOCK = 0x40000039,

	KEYBOARD_F1 = 0x4000003A,
	KEYBOARD_F2 = 0x4000003B,
	KEYBOARD_F3 = 0x4000003C,
	KEYBOARD_F4 = 0x4000003D,
	KEYBOARD_F5 = 0x4000003E,
	KEYBOARD_F6 = 0x4000003F,
	KEYBOARD_F7 = 0x40000040,
	KEYBOARD_F8 = 0x40000041,
	KEYBOARD_F9 = 0x40000042,
	KEYBOARD_F10 = 0x40000043,
	KEYBOARD_F11 = 0x40000044,
	KEYBOARD_F12 = 0x40000045,

	KEYBOARD_PRINTSCREEN = 0x40000046,
	KEYBOARD_SCROLLLOCK = 0x40000047,
	KEYBOARD_PAUSE = 0x40000048,
	KEYBOARD_INSERT = 0x40000049,
	KEYBOARD_HOME = 0x4000004A,
	KEYBOARD_PAGEUP = 0x4000004B,
	KEYBOARD_DELETE = '\177',
	KEYBOARD_END = 0x4000004D,
	KEYBOARD_PAGEDOWN = 0x4000004E,
	KEYBOARD_RIGHT = 0x4000004F,
	KEYBOARD_LEFT = 0x40000050,
	KEYBOARD_DOWN = 0x40000051,
	KEYBOARD_UP = 0x40000052,

	KEYBOARD_NUMLOCKCLEAR = 0x40000053,
	KEYBOARD_KP_DIVIDE = 0x40000054,
	KEYBOARD_KP_MULTIPLY = 0x40000055,
	KEYBOARD_KP_MINUS = 0x40000056,
	KEYBOARD_KP_PLUS = 0x40000057,
	KEYBOARD_KP_ENTER = 0x40000058,
	KEYBOARD_KP_1 = 0x40000059,
	KEYBOARD_KP_2 = 0x4000005A,
	KEYBOARD_KP_3 = 0x4000005B,
	KEYBOARD_KP_4 = 0x4000005C,
	KEYBOARD_KP_5 = 0x4000005D,
	KEYBOARD_KP_6 = 0x4000005E,
	KEYBOARD_KP_7 = 0x4000005F,
	KEYBOARD_KP_8 = 0x40000060,
	KEYBOARD_KP_9 = 0x40000061,
	KEYBOARD_KP_0 = 0x40000062,
	KEYBOARD_KP_PERIOD = 0x40000063,

	KEYBOARD_APPLICATION = 0x40000065,
	KEYBOARD_POWER = 0x40000066,
	KEYBOARD_KP_EQUALS = 0x40000067,
	KEYBOARD_F13 = 0x40000068,
	KEYBOARD_F14 = 0x40000069,
	KEYBOARD_F15 = 0x4000006A,
	KEYBOARD_F16 = 0x4000006B,
	KEYBOARD_F17 = 0x4000006C,
	KEYBOARD_F18 = 0x4000006D,
	KEYBOARD_F19 = 0x4000006E,
	KEYBOARD_F20 = 0x4000006F,
	KEYBOARD_F21 = 0x40000070,
	KEYBOARD_F22 = 0x40000071,
	KEYBOARD_F23 = 0x40000072,
	KEYBOARD_F24 = 0x40000073,
	KEYBOARD_EXECUTE = 0x40000074,
	KEYBOARD_HELP = 0x40000075,
	KEYBOARD_MENU = 0x40000076,
	KEYBOARD_SELECT = 0x40000077,
	KEYBOARD_STOP = 0x40000078,
	KEYBOARD_AGAIN = 0x40000079,
	KEYBOARD_UNDO = 0x4000007A,
	KEYBOARD_CUT = 0x4000007B,
	KEYBOARD_COPY = 0x4000007C,
	KEYBOARD_PASTE = 0x4000007D,
	KEYBOARD_FIND = 0x4000007E,
	KEYBOARD_MUTE = 0x4000007F,
	KEYBOARD_VOLUMEUP = 0x40000080,
	KEYBOARD_VOLUMEDOWN = 0x40000081,
	KEYBOARD_KP_COMMA = 0x40000085,
	KEYBOARD_KP_EQUALSAS400 = 0x40000086,

	KEYBOARD_ALTERASE = 0x40000099,
	KEYBOARD_SYSREQ = 0x4000009A,
	KEYBOARD_CANCEL = 0x4000009B,
	KEYBOARD_CLEAR = 0x4000009C,
	KEYBOARD_PRIOR = 0x4000009D,
	KEYBOARD_RETURN2 = 0x4000009E,
	KEYBOARD_SEPARATOR = 0x4000009F,
	KEYBOARD_OUT = 0x400000A0,
	KEYBOARD_OPER = 0x400000A1,
	KEYBOARD_CLEARAGAIN = 0x400000A2,
	KEYBOARD_CRSEL = 0x400000A3,
	KEYBOARD_EXSEL = 0x400000A4,

	KEYBOARD_KP_00 = 0x400000B0,
	KEYBOARD_KP_000 = 0x400000B1,
	KEYBOARD_THOUSANDSSEPARATOR = 0x400000B2,
	KEYBOARD_DECIMALSEPARATOR = 0x400000B3,
	KEYBOARD_CURRENCYUNIT = 0x400000B4,
	KEYBOARD_CURRENCYSUBUNIT = 0x400000B5,
	KEYBOARD_KP_LEFTPAREN = 0x400000B6,
	KEYBOARD_KP_RIGHTPAREN = 0x400000B7,
	KEYBOARD_KP_LEFTBRACE = 0x400000B8,
	KEYBOARD_KP_RIGHTBRACE = 0x400000B9,
	KEYBOARD_KP_TAB = 0x400000BA,
	KEYBOARD_KP_BACKSPACE = 0x400000BB,
	KEYBOARD_KP_A = 0x400000BC,
	KEYBOARD_KP_B = 0x400000BD,
	KEYBOARD_KP_C = 0x400000BE,
	KEYBOARD_KP_D = 0x400000BF,
	KEYBOARD_KP_E = 0x400000C0,
	KEYBOARD_KP_F = 0x400000C1,
	KEYBOARD_KP_XOR = 0x400000C2,
	KEYBOARD_KP_POWER = 0x400000C3,
	KEYBOARD_KP_PERCENT = 0x400000C4,
	KEYBOARD_KP_LESS = 0x400000C5,
	KEYBOARD_KP_GREATER = 0x400000C6,
	KEYBOARD_KP_AMPERSAND = 0x400000C7,
	KEYBOARD_KP_DBLAMPERSAND = 0x400000C8,
	KEYBOARD_KP_VERTICALBAR = 0x400000C9,
	KEYBOARD_KP_DBLVERTICALBAR = 0x400000CA,
	KEYBOARD_KP_COLON = 0x400000CB,
	KEYBOARD_KP_HASH = 0x400000CC,
	KEYBOARD_KP_SPACE = 0x400000CD,
	KEYBOARD_KP_AT = 0x400000CE,
	KEYBOARD_KP_EXCLAM = 0x400000CF,
	KEYBOARD_KP_MEMSTORE = 0x400000D0,
	KEYBOARD_KP_MEMRECALL = 0x400000D1,
	KEYBOARD_KP_MEMCLEAR = 0x400000D2,
	KEYBOARD_KP_MEMADD = 0x400000D3,
	KEYBOARD_KP_MEMSUBTRACT = 0x400000D4,
	KEYBOARD_KP_MEMMULTIPLY = 0x400000D5,
	KEYBOARD_KP_MEMDIVIDE = 0x400000D6,
	KEYBOARD_KP_PLUSMINUS = 0x400000D7,
	KEYBOARD_KP_CLEAR = 0x400000D8,
	KEYBOARD_KP_CLEARENTRY = 0x400000D9,
	KEYBOARD_KP_BINARY = 0x400000DA,
	KEYBOARD_KP_OCTAL = 0x400000DB,
	KEYBOARD_KP_DECIMAL = 0x400000DC,
	KEYBOARD_KP_HEXADECIMAL = 0x400000DD,

	KEYBOARD_LCTRL = 0x400000E0,
	KEYBOARD_LSHIFT = 0x400000E1,
	KEYBOARD_LALT = 0x400000E2,
	KEYBOARD_LGUI = 0x400000E3,
	KEYBOARD_RCTRL = 0x400000E4,
	KEYBOARD_RSHIFT = 0x400000E5,
	KEYBOARD_RALT = 0x400000E6,
	KEYBOARD_RGUI = 0x400000E7,

	KEYBOARD_MODE = 0x40000101,

	KEYBOARD_AUDIONEXT = 0x40000102,
	KEYBOARD_AUDIOPREV = 0x40000103,
	KEYBOARD_AUDIOSTOP = 0x40000104,
	KEYBOARD_AUDIOPLAY = 0x40000105,
	KEYBOARD_AUDIOMUTE = 0x40000106,
	KEYBOARD_MEDIASELECT = 0x40000107,
	KEYBOARD_WWW = 0x40000108,
	KEYBOARD_MAIL = 0x40000109,
	KEYBOARD_CALCULATOR = 0x4000010A,
	KEYBOARD_COMPUTER = 0x4000010B,
	KEYBOARD_AC_SEARCH = 0x4000010C,
	KEYBOARD_AC_HOME = 0x4000010D,
	KEYBOARD_AC_BACK = 0x4000010E,
	KEYBOARD_AC_FORWARD = 0x4000010F,
	KEYBOARD_AC_STOP = 0x40000110,
	KEYBOARD_AC_REFRESH = 0x40000111,
	KEYBOARD_AC_BOOKMARKS = 0x40000112,

	KEYBOARD_BRIGHTNESSDOWN = 0x40000113,
	KEYBOARD_BRIGHTNESSUP = 0x40000114,
	KEYBOARD_DISPLAYSWITCH = 0x40000115,
	KEYBOARD_KBDILLUMTOGGLE = 0x40000116,
	KEYBOARD_KBDILLUMDOWN = 0x40000117,
	KEYBOARD_KBDILLUMUP = 0x40000118,
	KEYBOARD_EJECT = 0x40000119,
	KEYBOARD_SLEEP = 0x4000011A
};
std::string getKeyboardKeyAsString(KeyboardKey key);

//MOUSE
enum MouseButton : unsigned int
{
	MOUSEBUTTON_LEFT = 1,
	MOUSEBUTTON_MIDDLE = 2,
	MOUSEBUTTON_RIGHT = 3,
	MOUSEBUTTON_X1 = 4, //Sidebuttons?
	MOUSEBUTTON_X2 = 5,
	//Are there more mouse buttons?
};
enum MouseWheel : int
{
	MOUSEWHEEL_UP = 1,
	MOUSEWHEEL_DOWN = -1,
};

//GAMEPAD
enum GamepadButton : int
{
	GAMEPAD_BUTTON_INVALID = -1,
	GAMEPAD_BUTTON_A,
	GAMEPAD_BUTTON_B,
	GAMEPAD_BUTTON_X,
	GAMEPAD_BUTTON_Y,
	GAMEPAD_BUTTON_BACK,
	GAMEPAD_BUTTON_GUIDE,
	GAMEPAD_BUTTON_START,
	GAMEPAD_BUTTON_LEFTSTICK,
	GAMEPAD_BUTTON_RIGHTSTICK,
	GAMEPAD_BUTTON_LEFTSHOULDER,
	GAMEPAD_BUTTON_RIGHTSHOULDER,
	GAMEPAD_BUTTON_DPAD_UP,
	GAMEPAD_BUTTON_DPAD_DOWN,
	GAMEPAD_BUTTON_DPAD_LEFT,
	GAMEPAD_BUTTON_DPAD_RIGHT,
	GAMEPAD_BUTTON_MAX
};
enum GamepadAxis : int
{
	GAMEPAD_AXIS_INVALID = -1,
	GAMEPAD_AXIS_LEFTX,
	GAMEPAD_AXIS_LEFTY,
	GAMEPAD_AXIS_RIGHTX,
	GAMEPAD_AXIS_RIGHTY,
	GAMEPAD_AXIS_TRIGGERLEFT,
	GAMEPAD_AXIS_TRIGGERRIGHT,
	GAMEPAD_AXIS_MAX
};
enum GamepadHatPosition
{
	GAMEPAD_HAT_INVALID = -1,
	GAMEPAD_HAT_CENTERED = 0x00,
	GAMEPAD_HAT_UP = 0x01,
	GAMEPAD_HAT_RIGHT = 0x02,
	GAMEPAD_HAT_DOWN = 0x04,
	GAMEPAD_HAT_LEFT = 0x08,
	GAMEPAD_HAT_RIGHTUP = (GAMEPAD_HAT_RIGHT | GAMEPAD_HAT_UP),
	GAMEPAD_HAT_RIGHTDOWN = (GAMEPAD_HAT_RIGHT | GAMEPAD_HAT_DOWN),
	GAMEPAD_HAT_LEFTUP = (GAMEPAD_HAT_LEFT | GAMEPAD_HAT_UP),
	GAMEPAD_HAT_LEFTDOWN = (GAMEPAD_HAT_LEFT | GAMEPAD_HAT_DOWN),
};