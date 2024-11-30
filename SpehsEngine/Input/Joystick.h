#pragma once

#include "SpehsEngine/Input/GUID.h"

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;
typedef int32_t SDL_JoystickID;


namespace se
{
	namespace input
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

			InputManager& inputManager;
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
			Joystick(InputManager& inputManager, int index);
			~Joystick();
			friend class InputManager;
		};
	}
}
