#pragma once

#include <unordered_map>
#include <string>

#include "SpehsEngine/Input/InputEnumerations.h"
#include "SpehsEngine/Input/InputState.h"
#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Input/GUID.h"

#define SINT16_MIN -32768
#define SINT16_MAX  32767

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
		void update(InputState& inputState);

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
		spehs::vec2 getMouseCoords() const { return mouseCoords; }
		spehs::vec2 getMouseMovement() const { return mouseMovement; }
		float getMouseX() const { return mouseCoords.x; }
		float getMouseY() const { return mouseCoords.y; }
		float getMouseMovementX() const { return mouseMovement.x; }
		float getMouseMovementY() const { return mouseMovement.y; }
		int getMouseWheelDelta() const { return mouseWheelDelta; }
		bool mouseCollision(const spehs::vec4& AABB_leftX_bottomY_width_height) const;
		bool mouseCollision(const spehs::vec2& minAABB, const spehs::vec2& maxAABB) const;
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
		spehs::ivec2 mouseCoords;
		spehs::ivec2 mouseMovement;
		
		//Joystick(s)
		void joystickConnected();
		void joystickDisconnected();
		void createJoystick(int index);
		void deleteJoystick(Joystick* ptr);
		int joystickCount = 0;
	};
}
extern spehs::InputManager* inputManager;
