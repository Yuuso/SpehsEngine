#pragma once

#include <unordered_map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "SpehsEngine/Input/InputEnumerations.h"
#include "SpehsEngine/Input/GUID.h"
#include "SpehsEngine/Input/Joystick.h"

#define SINT16_MIN -32768
#define SINT16_MAX  32767

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;
typedef int32_t SDL_JoystickID;

namespace se
{
	namespace input
	{
		class EventCatcher;

		class InputManager
		{
		public:
			InputManager();
			~InputManager();

			void update(EventCatcher& _eventCatcher);

			//manging keyboard
			void pressKey(unsigned int keyID);
			void releaseKey(unsigned int keyID);
			bool isKeyDown(unsigned int keyID) const;
			bool isKeyPressed(unsigned int keyID) const;
			bool isKeyReleased(unsigned int keyID) const;

			//Special input query
			bool isCtrlDown() const;
			bool isCtrlPressed() const;
			bool isCtrlReleased() const;

			bool isShiftDown() const;
			bool isShiftPressed() const;
			bool isShiftReleased() const;

			//Managing mouse
			void setMouseCoords(int _x, int _y);
			void lockMouse(const bool _value);
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
			std::string getDroppedFilePath() { std::string path = droppedFilePath; droppedFilePath.clear();/*Reset dropped file path*/ return path; }

			//Public access members
			std::vector<Joystick*> joysticks;
			KeyboardKey latestKeyboardPress = KeyboardKey::KEYBOARD_UNKNOWN;///< Latest key pressed. Reset for each update (0) if nothing was pressed during that update
			KeyboardKey latestMouseButtonPress = KEYBOARD_UNKNOWN;///< Latest mouse buton pressed. Reset for each update (0) if nothing was pressed during that update
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
			glm::vec2 mouseCoords;
			glm::vec2 mouseMovement;

			//Joystick(s)
			void joystickConnected();
			void joystickDisconnected();
			void createJoystick(int index);
			void deleteJoystick(Joystick* ptr);
			int joystickCount = 0;
		};
	}
}
