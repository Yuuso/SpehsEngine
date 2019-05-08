#pragma once
#include "SpehsEngine/Input/Event.h"
#include "boost/signals2.hpp"
#include "glm/vec2.hpp"
#include <vector>

namespace se
{
	namespace input
	{
		class EventCatcher;

		/*
			== Summary ==
			Passes catched input events to signal listeners in prioritized order.
			Upon calling update, the signals are called in order of appearance.
			If a listener returns true for the signal callback, the event is used and not passed to listeners below.
			If multiple listeners listen to a signal with the same priority, each will receive the callback and have the right to use it.

			== Core use guidelines ==
			== Point & mouse click ==
			1. Add receivingMouseHover boolean.
			2. Bind to pre-update, mouse hover and mouse butuon press signals with any priority.
			3. In the pre-update signal, set receivingMouseHover to false.
			4. If mouse hover callback is made, set receivingMouseHover to true and return true.
			5. If mouse button pressed callback is made, consume it on the condition that receivingMouseHover is set to true.

		*/
		class EventSignaler
		{
		public:
			static const int minPriority = std::numeric_limits<int>::min();
			static const int maxPriority = std::numeric_limits<int>::max();
		public:
			EventSignaler();

			void signalEvents(EventCatcher& eventCatcher);
			
			void connectToPreUpdateSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void()>& callback) { scopedConnection = preUpdateSignal.connect(callback); }
			void connectToPostUpdateSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void()>& callback) { scopedConnection = postUpdateSignal.connect(callback); }
			void connectToKeyboardPressSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const KeyboardPressEvent&)>& callback, const int priority);
			void connectToKeyboardDownSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const KeyboardDownEvent&)>& callback, const int priority);
			void connectToKeyboardReleaseSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const KeyboardReleaseEvent&)>& callback, const int priority);
			void connectToMouseHoverSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseHoverEvent&)>& callback, const int priority);
			void connectToMouseButtonPressSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseButtonPressEvent&)>& callback, const int priority);
			void connectToMouseButtonDownSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseButtonDownEvent&)>& callback, const int priority);
			void connectToMouseButtonReleaseSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseButtonReleaseEvent&)>& callback, const int priority);
			void connectToMouseMotionSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseMotionEvent&)>& callback, const int priority);
			void connectToMouseWheelSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const MouseWheelEvent&)>& callback, const int priority);
			//TODO: joysticks...
			void connectToQuitSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const QuitEvent&)>& callback, const int priority);
			void connectToFileDropSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const FileDropEvent&)>& callback, const int priority);

		private:
			template<typename EventType>
			struct PrioritizedEventSignal
			{
				struct Combiner
				{
					typedef bool result_type;
					template<typename InputIterator>
					bool operator()(InputIterator it, InputIterator end) const
					{
						if (it == end)
							return false;
						while (it != end)
						{
							if (*it++)
								return true;
						}
						return false;
					}
				};
				boost::signals2::signal<bool(const EventType&), Combiner> signal;
				int priority = 0;
			};
			template<typename EventType>
			void connectToEventSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<bool(const EventType&)>& callback, const int priority, std::vector<std::unique_ptr<PrioritizedEventSignal<EventType>>>& signals)
			{
				scopedConnection.disconnect();
				const std::vector<std::unique_ptr<PrioritizedEventSignal<EventType>>>::iterator it = std::find_if(signals.begin(), signals.end(), [priority](const std::unique_ptr<PrioritizedEventSignal<EventType>>& signal)->bool { return priority == signal->priority; });
				if (it == signals.end())
				{
					signals.push_back(std::unique_ptr<PrioritizedEventSignal<EventType>>(new PrioritizedEventSignal<EventType>()));
					signals.back()->priority = priority;
					scopedConnection = signals.back()->signal.connect(callback);
					std::sort(signals.begin(), signals.end(), [](const std::unique_ptr<PrioritizedEventSignal<EventType>>& _a, const std::unique_ptr<PrioritizedEventSignal<EventType>>&_b) { return _a->priority > _b->priority; });
				}
				else
				{
					if (priority != maxPriority && priority != minPriority && (*it)->signal.num_slots() > 0)
					{
						log::warning("Another source has already connected to this signal with the same priority");
					}
					scopedConnection = (*it)->signal.connect(callback);
				}
			}

			boost::signals2::signal<void(void)> preUpdateSignal;
			boost::signals2::signal<void(void)> postUpdateSignal;
			std::vector<std::unique_ptr<PrioritizedEventSignal<KeyboardPressEvent>>> keyboardPressSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<KeyboardDownEvent>>> keyboardDownSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<KeyboardReleaseEvent>>> keyboardReleaseSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseHoverEvent>>> mouseHoverSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseButtonPressEvent>>> mouseButtonPressSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseButtonDownEvent>>> mouseButtonDownSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseButtonReleaseEvent>>> mouseButtonReleaseSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseMotionEvent>>> mouseMotionSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseWheelEvent>>> mouseWheelSignals;
			//std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickButtonPressEvent>>> joystickButtonPressSignals;
			//std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickButtonDownEvent>>> joystickButtonDownSignals;
			//std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickButtonReleaseEvent>>> joystickButtonReleaseSignals;
			//std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickAxisEvent>>> joystickAxisSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<QuitEvent>>> quitSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<FileDropEvent>>> fileDropSignals;
		};
	}
}

/*
	Copy & paste cheatsheet
	For any class wishing to listen for any/all of the following events.

	//.h
	#include "SpehsEngine/Input/Event.h"
	#include "boost/signals2.hpp"

	void eventSignalerPreUpdateCallback();
	void eventSignalerPostUpdateCallback();
	bool keyboardPressCallback(const se::input::KeyboardPressEvent& event);
	bool keyboardDownCallback(const se::input::KeyboardDownEvent& event);
	bool keyboardReleaseCallback(const se::input::KeyboardReleaseEvent& event);
	bool mouseHoverCallback(const se::input::MouseHoverEvent& event);
	bool mouseButtonPressCallback(const se::input::MouseButtonPressEvent& event);
	bool mouseButtonDownCallback(const se::input::MouseButtonDownEvent& event);
	bool mouseButtonReleaseCallback(const se::input::MouseButtonReleaseEvent& event);
	bool mouseMotionCallback(const se::input::MouseMotionEvent& event);
	bool mouseWheelCallback(const se::input::MouseWheelEvent& event);

	boost::signals2::scoped_connection eventSignalerPreUpdateConnection;
	boost::signals2::scoped_connection eventSignalerPostUpdateConnection;
	boost::signals2::scoped_connection keyboardPressConnection;
	boost::signals2::scoped_connection keyboardDownConnection;
	boost::signals2::scoped_connection keyboardReleaseConnection;
	boost::signals2::scoped_connection mouseHoverConnection;
	boost::signals2::scoped_connection mouseButtonPressConnection;
	boost::signals2::scoped_connection mouseButtonDownConnection;
	boost::signals2::scoped_connection mouseButtonReleaseConnection;
	boost::signals2::scoped_connection mouseMotionConnection;
	boost::signals2::scoped_connection mouseWheelConnection;
	
	//.cpp
	#include "boost/bind.hpp"
	#include "SpehsEngine/Input/EventSignaler.h"
	const int inputPriority = INPUT_PRIORITY;
	eventSignaler.connectToPreUpdateSignal(eventSignalerPreUpdateConnection, boost::bind(&MyClass::eventSignalerPreUpdateCallback, this));
	eventSignaler.connectToPostUpdateSignal(eventSignalerPostUpdateConnection, boost::bind(&MyClass::eventSignalerPostUpdateCallback, this));
	eventSignaler.connectToKeyboardPressSignal(keyboardPressConnection, boost::bind(&MyClass::keyboardPressCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToKeyboardDownSignal(keyboardDownConnection, boost::bind(&MyClass::keyboardDownCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToKeyboardReleaseSignal(keyboardReleaseConnection, boost::bind(&MyClass::keyboardReleaseCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseHoverSignal(mouseWheelConnection, boost::bind(&MyClass::mouseHoverCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseButtonPressSignal(mouseButtonPressConnection, boost::bind(&MyClass::mouseButtonPressCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseButtonDownSignal(mouseButtonDownConnection, boost::bind(&MyClass::mouseButtonDownCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseButtonReleaseSignal(mouseButtonReleaseConnection, boost::bind(&MyClass::mouseButtonReleaseCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseMotionSignal(mouseMotionConnection, boost::bind(&MyClass::mouseMotionCallback, this, boost::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseWheelSignal(mouseWheelConnection, boost::bind(&MyClass::mouseWheelCallback, this, boost::placeholders::_1), inputPriority);
	
	void MyClass::eventSignalerPreUpdateCallback()
	{
		
	}

	void MyClass::eventSignalerPostUpdateCallback()
	{
		
	}

	bool MyClass::keyboardPressCallback(const se::input::KeyboardPressEvent& event)
	{
		return false;
	}

	bool MyClass::keyboardDownCallback(const se::input::KeyboardDownEvent& event)
	{
		return false;
	}

	bool MyClass::keyboardReleaseCallback(const se::input::KeyboardReleaseEvent& event)
	{
		return false;
	}

	bool MyClass::mouseHoverCallback(const se::input::MouseHoverEvent& event)
	{
		return false;
	}

	bool MyClass::mouseButtonPressCallback(const se::input::MouseButtonPressEvent& event)
	{
		return false;
	}

	bool MyClass::mouseButtonDownCallback(const se::input::MouseButtonDownEvent& event)
	{
		return false;
	}

	bool MyClass::mouseButtonReleaseCallback(const se::input::MouseButtonReleaseEvent& event)
	{
		return false;
	}

	bool MyClass::mouseMotionCallback(const se::input::MouseMotionEvent& event)
	{
		return false;
	}

	bool MyClass::mouseWheelCallback(const se::input::MouseWheelEvent& event)
	{
		return false;
	}
*/
