#include "stdafx.h"
#include "SpehsEngine/Input/EventCatcher.h"

#include "SpehsEngine/Input/MouseUtilityFunctions.h"
#include "SpehsEngine/Input/KeyUtilityFunctions.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SDL/SDL.h"
#include "SDL/SDL_joystick.h"
#include "SDL/SDL_events.h"

namespace se
{
	namespace input
	{
		EventCatcher::EventCatcher()
		{

		}

		void EventCatcher::pollEvents()
		{
			// Clear all previous events
			keyboardPressEvents.clear();
			keyboardDownEvents.clear();
			keyboardReleaseEvents.clear();
			mouseHoverEvents.clear();
			mouseButtonPressEvents.clear();
			mouseButtonDownEvents.clear();
			mouseButtonReleaseEvents.clear();
			mouseMotionEvents.clear();
			mouseWheelEvents.clear();
			//joystickButtonPressEvents.clear();
			//joystickButtonDownEvents.clear();
			//joystickButtonReleaseEvents.clear();
			//joystickAxisEvents.clear();
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
							}
							break;
						}
						//SDL_TEXTEDITING
						//SDL_TEXTINPUT
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
							mouseWheelEvents.back().wheelDelta = int(sdlEvent.wheel.y);
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

			//Handle mouse motion
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

			//Detect quietly released held keys
			for (std::unordered_set<Key>::iterator it = heldKeyboardKeys.begin(); it != heldKeyboardKeys.end();)
			{
				if (!isKeyDown(*it))
				{
					keyboardReleaseEvents.push_back(KeyboardReleaseEvent());
					keyboardReleaseEvents.back().key = *it;
					heldKeyboardKeys.erase(it);
					it = heldKeyboardKeys.begin(); // Erase invalidates iterators, need to restart
				}
				else
				{
					it++;
				}
			}

			//Detect quietly released held mouse buttons
			for (std::unordered_set<MouseButton>::iterator it = heldMouseButtons.begin(); it != heldMouseButtons.end();)
			{
				if (!isMouseButtonDown(*it))
				{
					mouseButtonReleaseEvents.push_back(MouseButtonReleaseEvent());
					mouseButtonReleaseEvents.back().button = *it;
					heldMouseButtons.erase(it);
					it = heldMouseButtons.begin(); // Erase invalidates iterators, need to restart
				}
				else
				{
					it++;
				}
			}

			//Held keyboard keys -> keyboard key down events
			for (std::unordered_set<Key>::iterator it = heldKeyboardKeys.begin(); it != heldKeyboardKeys.end(); it++)
			{
				keyboardDownEvents.push_back(KeyboardDownEvent());
				keyboardDownEvents.back().key = *it;
			}

			//Held mouse buttons -> mouse button down events
			for (std::unordered_set<MouseButton>::iterator it = heldMouseButtons.begin(); it != heldMouseButtons.end(); it++)
			{
				mouseButtonDownEvents.push_back(MouseButtonDownEvent());
				mouseButtonDownEvents.back().button = *it;
			}

			//Auto-generate the mouse hover event
			mouseHoverEvents.push_back(MouseHoverEvent());
			mouseHoverEvents.back().position = previousMousePosition;
		}
	}
}
