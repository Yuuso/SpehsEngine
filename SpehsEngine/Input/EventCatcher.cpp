#include "stdafx.h"
#include "SpehsEngine/Input/EventCatcher.h"

#include "SpehsEngine/Core/STLUnorderedMapUtilityFunctions.h"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"
#include "SpehsEngine/Input/KeyUtilityFunctions.h"
#include "SpehsEngine/Input/JoystickUtilityFunctions.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SDL/SDL.h"
#include "SDL/SDL_joystick.h"
#include "SDL/SDL_events.h"

#pragma optimize("", off) // nocommit
namespace se
{
	namespace input
	{
		EventCatcher::EventCatcher()
		{

		}

		void EventCatcher::pollEvents()
		{
			SE_SCOPE_PROFILER();
			// Clear all previous events
			keyboardEvents.clear();
			mouseHoverEvents.clear();
			textInputEvents.clear();
			mouseButtonEvents.clear();
			mouseMotionEvents.clear();
			mouseWheelEvents.clear();
			joystickButtonEvents.clear();
			joystickAxisEvents.clear();
			joystickHatEvents.clear();
			quitEvents.clear();
			fileDropEvents.clear();

			// Poll all SDL events
			bool mouseMotionCaught = false;
			glm::vec2 mousePosition;
			SDL_Event sdlEvent;
			while (SDL_PollEvent(&sdlEvent))
			{
				switch (sdlEvent.type)
				{
				default: break;
				case SDL_KEYDOWN:
				{
					const Key key = Key(sdlEvent.key.keysym.sym);
					keyboardEvents.push_back(KeyboardEvent());
					keyboardEvents.back().type = KeyboardEvent::Type::Press;
					keyboardEvents.back().key = key;
					keyboardEvents.back().scancode = sdlEvent.key.keysym.scancode;
					heldKeyboardKeys.emplace(key);
				}
					break;
				case SDL_KEYUP:
				{
					/* Generate release event only if press event was generated! */
					const Key key = Key(sdlEvent.key.keysym.sym);
					const std::unordered_set<Key>::iterator it = heldKeyboardKeys.find(key);
					if (it != heldKeyboardKeys.end())
					{
						heldKeyboardKeys.erase(it);
						keyboardEvents.push_back(KeyboardEvent());
						keyboardEvents.back().type = KeyboardEvent::Type::Release;
						keyboardEvents.back().key = key;
						keyboardEvents.back().scancode = sdlEvent.key.keysym.scancode;
					}
				}
					break;
				case SDL_TEXTINPUT:
				{
					textInputEvents.push_back(TextInputEvent());
					const size_t length = strlen(sdlEvent.text.text);
					textInputEvents.back().buffer.resize(length);
					memcpy(textInputEvents.back().buffer.data(), sdlEvent.text.text, length);
				}
				break;
				case SDL_MOUSEBUTTONDOWN:
				{
					const MouseButton button = MouseButton(sdlEvent.button.button);
					mouseButtonEvents.push_back(MouseButtonEvent());
					mouseButtonEvents.back().type = MouseButtonEvent::Type::Press;
					mouseButtonEvents.back().button = button;
					heldMouseButtons.emplace(button);
				}
					break;
				case SDL_MOUSEBUTTONUP:
				{
					/* Generate release event only if press event was generated! */
					const MouseButton button = MouseButton(sdlEvent.button.button);
					const std::unordered_set<MouseButton>::iterator it = heldMouseButtons.find(button);
					if (it != heldMouseButtons.end())
					{
						heldMouseButtons.erase(it);
						mouseButtonEvents.push_back(MouseButtonEvent());
						mouseButtonEvents.back().type = MouseButtonEvent::Type::Release;
						mouseButtonEvents.back().button = button;
					}
				}
					break;
				case SDL_MOUSEMOTION:
					/*
						The mouse position in the event uses the top-left centered coordinate system.
						Let's just use the global getter instead to avoid window dependency.
					*/
					mousePosition = getMousePosition();
					mouseMotionCaught = true;
					break;
				case SDL_MOUSEWHEEL:
					mouseWheelEvents.push_back(MouseWheelEvent());
					mouseWheelEvents.back().delta.x = int(sdlEvent.wheel.x);
					mouseWheelEvents.back().delta.y = int(sdlEvent.wheel.y);
					break;
				case SDL_QUIT:
					quitEvents.push_back(QuitEvent());
					break;
				case SDL_DROPFILE:
					fileDropEvents.push_back(FileDropEvent());
					fileDropEvents.back().filepath = sdlEvent.drop.file;
					break;
				case SDL_JOYBUTTONDOWN:
				{
					const JoystickId joystickId(sdlEvent.jbutton.which);
					JoystickState& joystickState = joystickStates[joystickId];
					if (!joystickState.joystickGuid)
					{
						joystickState.joystickGuid = getJoystickGuid(joystickId);
					}
					joystickState.heldButtons.emplace(sdlEvent.jbutton.button);
					joystickButtonEvents.push_back(JoystickButtonEvent());
					joystickButtonEvents.back().joystickId = joystickId;
					joystickButtonEvents.back().joystickGuid = *joystickState.joystickGuid;
					joystickButtonEvents.back().type = JoystickButtonEvent::Type::Press;
					joystickButtonEvents.back().buttonIndex = sdlEvent.jbutton.button;
				}
					break;
				case SDL_JOYBUTTONUP:
				{
					/* Generate release event only if press event was generated! */
					const JoystickId joystickId(sdlEvent.jbutton.which);
					JoystickState& joystickState = joystickStates[joystickId];
					if (!joystickState.joystickGuid)
					{
						joystickState.joystickGuid = getJoystickGuid(joystickId);
					}
					std::unordered_set<uint8_t>::iterator it = joystickState.heldButtons.find(sdlEvent.jbutton.button);
					if (it != joystickState.heldButtons.end())
					{
						joystickState.heldButtons.erase(it);
						joystickButtonEvents.push_back(JoystickButtonEvent());
						joystickButtonEvents.back().type = JoystickButtonEvent::Type::Release;
						joystickButtonEvents.back().joystickId = joystickId;
						joystickButtonEvents.back().joystickGuid = *joystickState.joystickGuid;
						joystickButtonEvents.back().buttonIndex = sdlEvent.jbutton.button;
					}
				}
					break;
				case SDL_JOYAXISMOTION:
				{
					const JoystickId joystickId(sdlEvent.jaxis.which);
					joystickAxisEvents.push_back(JoystickAxisEvent());
					joystickAxisEvents.back().joystickId = joystickId;
					joystickAxisEvents.back().joystickGuid = getJoystickGuid(joystickId);
					joystickAxisEvents.back().axisIndex = sdlEvent.jaxis.axis;
					joystickAxisEvents.back().axisState = sdlEvent.jaxis.value;
				}
					break;
				case SDL_JOYHATMOTION:
				{
					const JoystickId joystickId(sdlEvent.jhat.which);
					joystickHatEvents.push_back(JoystickHatEvent());
					joystickHatEvents.back().joystickId = joystickId;
					joystickHatEvents.back().joystickGuid = getJoystickGuid(joystickId);
					joystickHatEvents.back().hatIndex = sdlEvent.jhat.hat;
					joystickHatEvents.back().joystickHatState = JoystickHatState(sdlEvent.jhat.value);
				}
					break;
				}
			}

			// Handle mouse motion
			if (mouseMotionCaught)
			{
				if (previousMousePositionSet)
				{
					mouseMotionEvents.push_back(MouseMotionEvent());
					mouseMotionEvents.back().position = mousePosition;
					mouseMotionEvents.back().previousPosition = previousMousePosition;
					mouseMotionEvents.back().translation = mousePosition - previousMousePosition;
				}
				previousMousePositionSet = true;
				previousMousePosition = mousePosition;
			}

			// Detect quietly released held keys
			for (std::unordered_set<Key>::iterator it = heldKeyboardKeys.begin(); it != heldKeyboardKeys.end();)
			{
				if (isKeyDown(*it))
				{
					it++;
				}
				else
				{
					keyboardEvents.push_back(KeyboardEvent());
					keyboardEvents.back().type = KeyboardEvent::Type::Release;
					keyboardEvents.back().key = *it;
					keyboardEvents.back().scancode = getScancode(*it);
					heldKeyboardKeys.erase(it);
					it = heldKeyboardKeys.begin(); // Erase invalidates iterators, need to restart
				}
			}

			// Detect quietly released held mouse buttons
			for (std::unordered_set<MouseButton>::iterator it = heldMouseButtons.begin(); it != heldMouseButtons.end();)
			{
				if (isMouseButtonDown(*it))
				{
					it++;
				}
				else
				{
					mouseButtonEvents.push_back(MouseButtonEvent());
					mouseButtonEvents.back().type = MouseButtonEvent::Type::Release;
					mouseButtonEvents.back().button = *it;
					heldMouseButtons.erase(it);
					it = heldMouseButtons.begin(); // Erase invalidates iterators, need to restart
				}
			}

			// Detect quietly released held joystick buttons
			for (std::unordered_map<const JoystickId, JoystickState>::iterator it1 = joystickStates.begin(); it1 != joystickStates.end();)
			{
				if (getJoystickConnected(it1->first))
				{
					bool advanceIt1 = true;
					for (std::unordered_set<uint8_t>::iterator it2 = it1->second.heldButtons.begin(); it2 != it1->second.heldButtons.end();)
					{
						if (getJoystickButtonState(it1->first, *it2))
						{
							it2++;
						}
						else
						{
							joystickButtonEvents.push_back(JoystickButtonEvent());
							joystickButtonEvents.back().joystickId = it1->first;
							joystickButtonEvents.back().joystickGuid = *it1->second.joystickGuid;
							joystickButtonEvents.back().buttonIndex = *it2;
							joystickButtonEvents.back().type = JoystickButtonEvent::Type::Release;
							if (it1->second.heldButtons.empty())
							{
								it1 = joystickStates.erase(it1);
								advanceIt1 = false;
								break;
							}
							else
							{
								it2++;
							}
						}
					}
					if (advanceIt1)
					{
						it1++;
					}
				}
				else
				{
					for (const uint8_t buttonIndex : it1->second.heldButtons)
					{
						joystickButtonEvents.push_back(JoystickButtonEvent());
						joystickButtonEvents.back().joystickId = it1->first;
						joystickButtonEvents.back().joystickGuid = *it1->second.joystickGuid;
						joystickButtonEvents.back().buttonIndex = buttonIndex;
						joystickButtonEvents.back().type = JoystickButtonEvent::Type::Release;
					}
					it1 = joystickStates.erase(it1);
				}
			}

			// Held keyboard keys -> keyboard key down events
			for (std::unordered_set<Key>::iterator it = heldKeyboardKeys.begin(); it != heldKeyboardKeys.end(); it++)
			{
				keyboardEvents.push_back(KeyboardEvent());
				keyboardEvents.back().type = KeyboardEvent::Type::Hold;
				keyboardEvents.back().key = *it;
				keyboardEvents.back().scancode = getScancode(*it);
			}

			// Held mouse buttons -> mouse button down events
			for (std::unordered_set<MouseButton>::iterator it = heldMouseButtons.begin(); it != heldMouseButtons.end(); it++)
			{
				mouseButtonEvents.push_back(MouseButtonEvent());
				mouseButtonEvents.back().type = MouseButtonEvent::Type::Hold;
				mouseButtonEvents.back().button = *it;
			}

			// Held joystick buttons -> joystick button down events
			for (std::unordered_map<JoystickId, JoystickState>::iterator it1 = joystickStates.begin(); it1 != joystickStates.end(); it1++)
			{
				for (std::unordered_set<uint8_t>::iterator it2 = it1->second.heldButtons.begin(); it2 != it1->second.heldButtons.end(); it2++)
				{
					joystickButtonEvents.push_back(JoystickButtonEvent());
					joystickButtonEvents.back().joystickId = it1->first;
					joystickButtonEvents.back().joystickGuid = *it1->second.joystickGuid;
					joystickButtonEvents.back().buttonIndex = *it2;
					joystickButtonEvents.back().type = JoystickButtonEvent::Type::Hold;
				}
			}

			// Auto-generate the mouse hover event
			mouseHoverEvents.push_back(MouseHoverEvent());
			mouseHoverEvents.back().position = previousMousePosition;
		}
	}
}
