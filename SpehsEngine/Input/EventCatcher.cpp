#include "stdafx.h"
#include "SpehsEngine/Input/EventCatcher.h"

#include "SpehsEngine/Rendering/Window.h"
#include "SDL/SDL.h"
#include "SDL/SDL_joystick.h"
#include "SDL/SDL_events.h"

namespace se
{
	namespace input
	{
		EventCatcher::EventCatcher(const rendering::Window& _window)
			: window(_window)
		{

		}

		void EventCatcher::update()
		{
			//Clear all previous events
			keyboardPressEvents.clear();
			keyboardReleaseEvents.clear();
			mouseButtonPressEvents.clear();
			mouseButtonReleaseEvents.clear();
			mouseMotionEvents.clear();
			mouseWheelEvents.clear();
			mouseHoverEvents.clear();
			//joystickButtonPressEvents.clear();
			//joystickButtonReleaseEvents.clear();
			//joystickAxisEvents.clear();
			quitEvents.clear();
			fileDropEvents.clear();

			//Poll all SDL events
			bool mouseMotionCaught = false;
			glm::vec2 mousePosition;
			SDL_Event sdlEvent;
			while (SDL_PollEvent(&sdlEvent))
			{
				switch (sdlEvent.type)
				{
				default: break;
				case SDL_KEYDOWN: keyboardPressEvents.push_back(KeyboardPressEvent(KeyboardKey(sdlEvent.key.keysym.sym))); break;
				case SDL_KEYUP: keyboardReleaseEvents.push_back(KeyboardReleaseEvent(KeyboardKey(sdlEvent.key.keysym.sym))); break;
				case SDL_MOUSEBUTTONDOWN: mouseButtonPressEvents.push_back(MouseButtonPressEvent(KeyboardKey(sdlEvent.button.button))); break;
				case SDL_MOUSEBUTTONUP: mouseButtonReleaseEvents.push_back(MouseButtonReleaseEvent(KeyboardKey(sdlEvent.button.button))); break;
				case SDL_MOUSEMOTION: mousePosition = glm::vec2(sdlEvent.motion.x, window.getHeight() - sdlEvent.motion.y); mouseMotionCaught = true; break;
				case SDL_MOUSEWHEEL: mouseWheelEvents.push_back(MouseWheelEvent(sdlEvent.wheel.y)); break;
				case SDL_QUIT: quitEvents.push_back(QuitEvent()); break;
				case SDL_DROPFILE: fileDropEvents.push_back(FileDropEvent(sdlEvent.drop.file)); break;
				}
			}

			//Handle mouse motion
			if (mouseMotionCaught)
			{
				if (previousMousePositionSet)
				{
					mouseMotionEvents.push_back(MouseMotionEvent(mousePosition, previousMousePosition));
				}
				previousMousePositionSet = true;
				previousMousePosition = mousePosition;
			}

			//Auto-generate the mouse hover event
			mouseHoverEvents.push_back(MouseHoverEvent());
		}
	}
}
