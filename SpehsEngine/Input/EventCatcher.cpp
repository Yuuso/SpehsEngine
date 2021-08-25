#include "stdafx.h"
#include "SpehsEngine/Input/EventCatcher.h"

#include "SpehsEngine/Core/STLUnorderedMapUtilityFunctions.h"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"
#include "SpehsEngine/Input/KeyUtilityFunctions.h"
#include "SpehsEngine/Input/JoystickUtilityFunctions.h"
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
			// Clear all previous events
			keyboardPressEvents.clear();
			keyboardDownEvents.clear();
			keyboardReleaseEvents.clear();
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

			// Get all non-window SDL events
			bool mouseMotionCaught = false;
			glm::vec2 mousePosition;

			SDL_PumpEvents();

			static const size_t MAX_EVENTS = 256;
			SDL_Event sdlEvents[MAX_EVENTS];

			const int numQuitEvents = SDL_PeepEvents(sdlEvents, (int)MAX_EVENTS, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_APP_DIDENTERFOREGROUND);
			if (numQuitEvents > 0)
			{
				for (size_t i = 0; i < (size_t)numQuitEvents; i++)
				{
					const SDL_Event& sdlEvent = sdlEvents[i];

					switch (sdlEvent.type)
					{
						// Quit event
						case SDL_QUIT:
						{
							quitEvents.push_back(QuitEvent());
							break;
						}
					}
				}
			}
			else if (numQuitEvents == -1)
			{
				std::string error = "EventCatcher: SDL_PeepEvents failed! ";
				error += SDL_GetError();
				log::error(error);
			}

			const int numInputEvents = SDL_PeepEvents(sdlEvents, (int)MAX_EVENTS, SDL_GETEVENT, SDL_KEYDOWN, SDL_LASTEVENT);
			if (numInputEvents > 0)
			{
				for (size_t i = 0; i < (size_t)numInputEvents; i++)
				{
					const SDL_Event& sdlEvent = sdlEvents[i];

					switch (sdlEvent.type)
					{
						// Keyboard events
						case SDL_KEYDOWN:
						{
							const Key key = Key(sdlEvent.key.keysym.sym);
							keyboardPressEvents.push_back(KeyboardPressEvent());
							keyboardPressEvents.back().key = key;
							keyboardPressEvents.back().scancode = sdlEvent.key.keysym.scancode;
							heldKeyboardKeys.emplace(key);
							break;
						}
						case SDL_KEYUP:
						{
							/* Generate release event only if press event was generated! */
							const Key key = Key(sdlEvent.key.keysym.sym);
							const std::unordered_set<Key>::iterator it = heldKeyboardKeys.find(key);
							if (it != heldKeyboardKeys.end())
							{
								heldKeyboardKeys.erase(it);
								keyboardReleaseEvents.push_back(KeyboardReleaseEvent());
								keyboardReleaseEvents.back().key = key;
								keyboardReleaseEvents.back().scancode = sdlEvent.key.keysym.scancode;
							}
							break;
						}
						case SDL_TEXTINPUT:
						{
							textInputEvents.push_back(TextInputEvent());
							const size_t length = strlen(sdlEvent.text.text);
							textInputEvents.back().buffer.resize(length);
							memcpy(textInputEvents.back().buffer.data(), sdlEvent.text.text, length);
						}
						break;
						//SDL_TEXTEDITING
						//SDL_KEYMAPCHANGED

						// Mouse events
						case SDL_MOUSEMOTION:
						{
							/*
								The mouse position in the event uses the top-left centered coordinate system.
								Let's just use the global getter instead to avoid window dependency.
							*/
							mousePosition = getMousePosition();
							mouseMotionCaught = true;
							break;
						}
						case SDL_MOUSEBUTTONDOWN:
						{
							const MouseButton button = MouseButton(sdlEvent.button.button);
							mouseButtonPressEvents.push_back(MouseButtonPressEvent());
							mouseButtonPressEvents.back().button = button;
							heldMouseButtons.emplace(button);
							break;
						}
						case SDL_MOUSEBUTTONUP:
						{
							/* Generate release event only if press event was generated! */
							const MouseButton button = MouseButton(sdlEvent.button.button);
							const std::unordered_set<MouseButton>::iterator it = heldMouseButtons.find(button);
							if (it != heldMouseButtons.end())
							{
								heldMouseButtons.erase(it);
								mouseButtonReleaseEvents.push_back(MouseButtonReleaseEvent());
								mouseButtonReleaseEvents.back().button = button;
							}
							break;
						}
						case SDL_MOUSEWHEEL:
						{
							mouseWheelEvents.push_back(MouseWheelEvent());
							mouseWheelEvents.back().delta.x = int(sdlEvent.wheel.x);
							mouseWheelEvents.back().delta.y = int(sdlEvent.wheel.y);
							break;
						}

						// Clipboard events
						//SDL_CLIPBOARDUPDATE

						// Drag and drop events
						case SDL_DROPFILE:
						{
							fileDropEvents.push_back(FileDropEvent());
							fileDropEvents.back().filepath = sdlEvent.drop.file;
							break;
						}
					}
				}
			}
			else if (numQuitEvents == -1)
			{
				std::string error = "EventCatcher: SDL_PeepEvents failed! ";
				error += SDL_GetError();
				log::error(error);
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
