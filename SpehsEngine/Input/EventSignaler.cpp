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

		void EventSignaler::signalEvents(EventCatcher& eventCatcher)
		{
			SE_SCOPE_PROFILER();
			preUpdateSignal();

			// Get events
			const std::vector<MouseHoverEvent>& mouseHoverEvents = eventCatcher.getMouseHoverEvents();
			const std::vector<MouseMotionEvent>& mouseMotionEvents = eventCatcher.getMouseMotionEvents();
			const std::vector<MouseWheelEvent>& mouseWheelEvents = eventCatcher.getMouseWheelEvents();
			const std::vector<MouseButtonEvent>& mouseButtonEvents = eventCatcher.getMouseButtonEvents();
			const std::vector<KeyboardEvent>& keyboardEvents = eventCatcher.getKeyboardEvents();
			const std::vector<TextInputEvent>& textInputEvents = eventCatcher.getTextInputEvents();
			const std::vector<JoystickButtonEvent>& joystickButtonEvents = eventCatcher.getJoystickButtonEvents();
			const std::vector<JoystickAxisEvent>& joystickAxisEvents = eventCatcher.getJoystickAxisEvents();
			const std::vector<JoystickHatEvent>& joystickHatEvents = eventCatcher.getJoystickHatEvents();
			const std::vector<QuitEvent>& quitEvents = eventCatcher.getQuitEvents();
			const std::vector<FileDropEvent>& fileDropEvents = eventCatcher.getFileDropEvents();

			// Signal events
			signalEventsImpl(mouseHoverEvents, mouseHoverSignals);
			signalEventsImpl(mouseMotionEvents, mouseMotionSignals);
			signalEventsImpl(mouseWheelEvents, mouseWheelSignals);
			signalEventsImpl(mouseButtonEvents, mouseButtonSignals);
			signalEventsImpl(keyboardEvents, keyboardSignals);
			signalEventsImpl(textInputEvents, textInputSignals);
			signalEventsImpl(joystickButtonEvents, joystickButtonSignals);
			signalEventsImpl(joystickAxisEvents, joystickAxisSignals);
			signalEventsImpl(joystickHatEvents, joystickHatSignals);
			signalEventsImpl(quitEvents, quitSignals);
			signalEventsImpl(fileDropEvents, fileDropSignals);

			postUpdateSignal();
		}

		void EventSignaler::connectToMouseHoverSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseHoverEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseHoverEvent>(scopedConnection, callback, priority, mouseHoverSignals);
		}

		void EventSignaler::connectToMouseMotionSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseMotionEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseMotionEvent>(scopedConnection, callback, priority, mouseMotionSignals);
		}

		void EventSignaler::connectToMouseWheelSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseWheelEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseWheelEvent>(scopedConnection, callback, priority, mouseWheelSignals);
		}

		void EventSignaler::connectToMouseButtonSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseButtonEvent&)>& callback, const int priority)
		{
			connectToEventSignal<MouseButtonEvent>(scopedConnection, callback, priority, mouseButtonSignals);
		}

		void EventSignaler::connectToKeyboardSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const KeyboardEvent&)>& callback, const int priority)
		{
			connectToEventSignal<KeyboardEvent>(scopedConnection, callback, priority, keyboardSignals);
		}

		void EventSignaler::connectToTextInputSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const TextInputEvent&)>& callback, const int priority)
		{
			connectToEventSignal<TextInputEvent>(scopedConnection, callback, priority, textInputSignals);
		}

		void EventSignaler::connectToJoystickButtonSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const JoystickButtonEvent&)>& callback, const int priority)
		{
			connectToEventSignal<JoystickButtonEvent>(scopedConnection, callback, priority, joystickButtonSignals);
		}

		void EventSignaler::connectToJoystickAxisSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const JoystickAxisEvent&)>& callback, const int priority)
		{
			connectToEventSignal<JoystickAxisEvent>(scopedConnection, callback, priority, joystickAxisSignals);
		}

		void EventSignaler::connectToJoystickHatSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const JoystickHatEvent&)>& callback, const int priority)
		{
			connectToEventSignal<JoystickHatEvent>(scopedConnection, callback, priority, joystickHatSignals);
		}

		void EventSignaler::connectToQuitSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const QuitEvent&)>& callback, const int priority)
		{
			connectToEventSignal<QuitEvent>(scopedConnection, callback, priority, quitSignals);
		}

		void EventSignaler::connectToFileDropSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const FileDropEvent&)>& callback, const int priority)
		{
			connectToEventSignal<FileDropEvent>(scopedConnection, callback, priority, fileDropSignals);
		}
	}
}
