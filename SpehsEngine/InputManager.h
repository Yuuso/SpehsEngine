#pragma once
#include <SDL/SDL_events.h>
#include <glm/vec2.hpp>
#include <unordered_map>
#include "GUID.h"
#define SINT16_MIN -32768
#define SINT16_MAX  32767
#define MOUSE_BUTTON_LEFT 1
#define MOUSE_BUTTON_MIDDLE 2
#define MOUSE_BUTTON_RIGHT 3
#define MOUSE_BUTTON_4 4
#define MOUSE_BUTTON_5 5

namespace SpehsEngine
{
	/**Joysticks can only be created during run time, they will not be removed but instead go into offline mode. \n
	This is because many parts of the program can be attached to a joystick pointer/reference and unknowingly could try to access removed joysticks state*/
	struct Joystick
	{
		~Joystick();
		Joystick(int index);

		/*Returns the axis state, range [-1, 1] NOTE: float precision...*/
		float getAxisState(int axisIndex);
		bool isButtonDown(int buttonIndex);
		bool isHatInPosition(int hatIndex, Uint8 position);
		int32_t queryButtonDown();///<Returns -1 if no button is pressed. Returns button index (>= 0) if a button is pressed. If multiple buttons are being pressed, returns the one with the lowerst index.
		int getGUIDIndex();///<Searches for joysticks with the same GUID as given joystick. Returns index of given joystick among devices with the same GUID

		GUID guid;
		SDL_Joystick* joystick;
		SDL_JoystickID ID;
		std::string name;

		int axisCount;
		int buttonCount;
		int hatCount;

		void goOffline();
		void goOnline(SDL_Joystick* newJs);
		bool offline;///<When joystick becomes desconnected during run time it goes into offline mode. In offline mode key pressed return false, axis querys return 0
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
		bool isKeyDown(unsigned int keyID);
		bool isKeyPressed(unsigned int keyID);

		//Managing mouse
		void setMouseCoords(int _x, int _y);

		//Managing joysticks
		Joystick* getJoystick(GUID guid, int preferredIndex = 0);
		int getGUIDIndex(int joystickIndex);///<Searches for joysticks with the same GUID as given joystick. Returns index of given joystick among devices with the same GUID

		//Getters
		glm::vec2 getMouseCoords() const { return mouseCoords; }
		glm::vec2 getMouseMovement(){ return mouseMovement; }
		float getMouseX(){ return mouseCoords.x; }
		float getMouseY(){ return mouseCoords.y; }
		float getMouseMovementX(){ return mouseMovement.x; }
		float getMouseMovementY(){ return mouseMovement.y; }
		int getMouseWheelDelta() const { return mouseWheelDelta; }
		bool isQuitRequested() const { return quitRequested; }

		//Public access members
		std::vector<Joystick*> joysticks;
		int32_t latestKeyboardPress;///< Latest key pressed. Reset for each update (0) if nothing was pressed during that update
		int32_t latestMouseButtonPress;///< Latest mouse buton pressed. Reset for each update (0) if nothing was pressed during that update

	private:
		SDL_Event mEvent;
		bool quitRequested = false;
		
		//Keyboard
		bool wasKeyDown(unsigned int keyID);
		std::unordered_map<unsigned int, bool> keyMap;
		std::unordered_map<unsigned int, bool> previousKeyMap;

		//Mouse
		glm::ivec2 mouseCoords;
		glm::ivec2 mouseMovement;
		int mouseWheelDelta = 0;
		
		//Joystick(s)
		void joystickConnected();
		void joystickDisconnected();
		void createJoystick(int index);
		void deleteJoystick(Joystick* ptr);
		int joystickCount = 0;

	};
}
extern SpehsEngine::InputManager* inputManager;