#include "stdafx.h"
#include "SpehsEngine/Input/EventSignaler.h"

#include "SpehsEngine/Input/EventCatcher.h"

namespace se
{
	namespace input
	{
		template<typename EventContainer, typename SignalContainer>
		void signalEventsImpl(const EventContainer& events, SignalContainer& signals)
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
						if (signals[s]->signal(events[e]))
						{
							//A connected receiver used the event, do not call receivers with a lower priority.
							break;
						}
						else
						{
							s++;
						}
					}
				}
			}
		}

		EventSignaler::EventSignaler()
		{
			
		}

		void EventSignaler::signalEvents(EventCatcher& eventCatcher)
		{
			preUpdateSignal();

			//Get events
			const std::vector<MouseHoverEvent>& mouseHoverEvents = eventCatcher.getMouseHoverEvents();
			const std::vector<KeyboardPressEvent>& keyboardPressEvents = eventCatcher.getKeyboardPressEvents();
			const std::vector<KeyboardDownEvent>& keyboardDownEvents = eventCatcher.getKeyboardDownEvents();
			const std::vector<KeyboardReleaseEvent>& keyboardReleaseEvents = eventCatcher.getKeyboardReleaseEvents();
			const std::vector<MouseButtonPressEvent>& mouseButtonPressEvents = eventCatcher.getMouseButtonPressEvents();
			const std::vector<MouseButtonDownEvent>& mouseButtonDownEvents = eventCatcher.getMouseButtonDownEvents();
			const std::vector<MouseButtonReleaseEvent>& mouseButtonReleaseEvents = eventCatcher.getMouseButtonReleaseEvents();
			const std::vector<MouseMotionEvent>& mouseMotionEvents = eventCatcher.getMouseMotionEvents();
			const std::vector<MouseWheelEvent>& mouseWheelEvents = eventCatcher.getMouseWheelEvents();
			//const std::vector<JoystickButtonPressEvent>& joystickButtonPressEvents = eventCatcher.get;
			//const std::vector<JoystickButtonDownEvent>& joystickButtonDownEvents = eventCatcher.get;
			//const std::vector<JoystickButtonReleaseEvent>& joystickButtonReleaseEvents = eventCatcher.get;
			//const std::vector<JoystickAxisEvent>& joystickAxisEvents = eventCatcher.get;
			const std::vector<QuitEvent>& quitEvents = eventCatcher.getQuitEvents();
			const std::vector<FileDropEvent>& fileDropEvents = eventCatcher.getFileDropEvents();
			
			//Signal events
			signalEventsImpl(keyboardPressEvents, keyboardPressSignals);
			signalEventsImpl(keyboardDownEvents, keyboardDownSignals);
			signalEventsImpl(keyboardReleaseEvents, keyboardReleaseSignals);
			signalEventsImpl(mouseHoverEvents, mouseHoverSignals);
			signalEventsImpl(mouseButtonPressEvents, mouseButtonPressSignals);
			signalEventsImpl(mouseButtonDownEvents, mouseButtonDownSignals);
			signalEventsImpl(mouseButtonReleaseEvents, mouseButtonReleaseSignals);
			signalEventsImpl(mouseMotionEvents, mouseMotionSignals);
			signalEventsImpl(mouseWheelEvents, mouseWheelSignals);
			//signalEventsImpl(joystickButtonPressEvents, joystickButtonPressSignals);
			//signalEventsImpl(joystickButtonDownEvents, joystickButtonDownSignals);
			//signalEventsImpl(joystickButtonReleaseEvents, joystickButtonReleaseSignals);
			//signalEventsImpl(joystickAxisEvents, joystickAxisSignals);

			postUpdateSignal();
		}

		void EventSignaler::connectToKeyboardPressSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const KeyboardPressEvent&)>& callback, const int priority)
		{
			connectToEventSignal<KeyboardPressEvent>(scopedConnection, callback, priority, keyboardPressSignals);
		}

		void EventSignaler::connectToKeyboardDownSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const KeyboardDownEvent&)>& callback, const int priority)
		{
			connectToEventSignal<KeyboardDownEvent>(scopedConnection, callback, priority, keyboardDownSignals);
		}

		void EventSignaler::connectToKeyboardReleaseSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const KeyboardReleaseEvent&)>& callback, const int priority)
		{
			connectToEventSignal<KeyboardReleaseEvent>(scopedConnection, callback, priority, keyboardReleaseSignals);
		}

		void EventSignaler::connectToMouseHoverSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const MouseHoverEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseHoverEvent>(scopedConnection, callback, priority, mouseHoverSignals);
		}

		void EventSignaler::connectToMouseButtonPressSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const MouseButtonPressEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseButtonPressEvent>(scopedConnection, callback, priority, mouseButtonPressSignals);
		}

		void EventSignaler::connectToMouseButtonDownSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const MouseButtonDownEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseButtonDownEvent>(scopedConnection, callback, priority, mouseButtonDownSignals);
		}

		void EventSignaler::connectToMouseButtonReleaseSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const MouseButtonReleaseEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseButtonReleaseEvent>(scopedConnection, callback, priority, mouseButtonReleaseSignals);
		}

		void EventSignaler::connectToMouseMotionSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const MouseMotionEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseMotionEvent>(scopedConnection, callback, priority, mouseMotionSignals);
		}

		void EventSignaler::connectToMouseWheelSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const MouseWheelEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseWheelEvent>(scopedConnection, callback, priority, mouseWheelSignals);
		}

		void EventSignaler::connectToQuitSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const QuitEvent&)>& callback, const int priority)
		{
			connectToEventSignal<QuitEvent>(scopedConnection, callback, priority, quitSignals);
		}

		void EventSignaler::connectToFileDropSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<bool(const FileDropEvent&)>& callback, const int priority)
		{
			connectToEventSignal<FileDropEvent>(scopedConnection, callback, priority, fileDropSignals);
		}
	}
}