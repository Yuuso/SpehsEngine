#include "stdafx.h"
#include "SpehsEngine/Input/EventSignaler.h"

#include "SpehsEngine/Input/EventCatcher.h"

namespace se
{
	namespace input
	{
		template<typename EventContainer, typename SignalContainer>
		void signalEvents(EventContainer& events, SignalContainer& signals)
		{
			for (size_t e = 0; e < events.size(); e++)
			{
				for (size_t s = 0; s < signals.size();)
				{
					if (signals[s]->signal.empty())
					{
						signals.erase(signals.begin() + s);
					}
					else
					{
						signals[s]->signal(events[e]);
						s++;
					}
				}
			}
		}

		EventSignaler::EventSignaler(EventCatcher& _eventCatcher)
			: eventCatcher(_eventCatcher)
		{
		
		}

		void EventSignaler::update()
		{
			//Get events
			std::vector<KeyboardPressEvent>& keyboardPressEvents = eventCatcher.getKeyboardPressEvents();
			std::vector<KeyboardReleaseEvent>& keyboardReleaseEvents = eventCatcher.getKeyboardReleaseEvents();
			std::vector<MouseButtonPressEvent>& mouseButtonPressEvents = eventCatcher.getMouseButtonPressEvents();
			std::vector<MouseButtonReleaseEvent>& mouseButtonReleaseEvents = eventCatcher.getMouseButtonReleaseEvents();
			std::vector<MouseMotionEvent>& mouseMotionEvents = eventCatcher.getMouseMotionEvents();
			std::vector<MouseWheelEvent>& mouseWheelEvents = eventCatcher.getMouseWheelEvents();
			std::vector<MouseHoverEvent>& mouseHoverEvents = eventCatcher.getMouseHoverEvents();
			//std::vector<JoystickButtonPressEvent>& joystickButtonPressEvents = eventCatcher.get;
			//std::vector<JoystickButtonReleaseEvent>& joystickButtonReleaseEvents = eventCatcher.get;
			//std::vector<JoystickAxisEvent>& joystickAxisEvents = eventCatcher.get;
						
			//Signal events
			signalEvents(keyboardPressEvents, keyboardPressSignals);
			signalEvents(keyboardReleaseEvents, keyboardReleaseSignals);
			signalEvents(mouseButtonPressEvents, mouseButtonPressSignals);
			signalEvents(mouseButtonReleaseEvents, mouseButtonReleaseSignals);
			signalEvents(mouseMotionEvents, mouseMotionSignals);
			signalEvents(mouseHoverEvents, mouseHoverSignals);
			signalEvents(mouseWheelEvents, mouseWheelSignals);
			//signalEvents(joystickButtonPressEvents, joystickButtonPressSignals);
			//signalEvents(joystickButtonReleaseEvents, joystickButtonReleaseSignals);
			//signalEvents(joystickAxisEvents, joystickAxisSignals);
		}

		void EventSignaler::connectToKeyboardPressEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(KeyboardPressEvent&)>& callback, const int priority)
		{
			connectToEventSignal<KeyboardPressEvent>(scopedConnection, callback, priority, keyboardPressSignals);
		}

		void EventSignaler::connectToKeyboardReleaseEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(KeyboardReleaseEvent&)>& callback, const int priority)
		{
			connectToEventSignal<KeyboardReleaseEvent>(scopedConnection, callback, priority, keyboardReleaseSignals);
		}

		void EventSignaler::connectToMouseButtonPressEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseButtonPressEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseButtonPressEvent>(scopedConnection, callback, priority, mouseButtonPressSignals);
		}

		void EventSignaler::connectToMouseButtonReleaseEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseButtonReleaseEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseButtonReleaseEvent>(scopedConnection, callback, priority, mouseButtonReleaseSignals);
		}

		void EventSignaler::connectToMouseMotionEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseMotionEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseMotionEvent>(scopedConnection, callback, priority, mouseMotionSignals);
		}

		void EventSignaler::connectToMouseHoverEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseHoverEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseHoverEvent>(scopedConnection, callback, priority, mouseHoverSignals);
		}

		void EventSignaler::connectToMouseWheelEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseWheelEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseWheelEvent>(scopedConnection, callback, priority, mouseWheelSignals);
		}
	}
}
