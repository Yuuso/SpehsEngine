#pragma once
#include "SpehsEngine/Input/Event.h"
#include "boost/signals.hpp"
#include "glm/vec2.hpp"
#include <vector>

namespace se
{
	namespace input
	{
		class EventCatcher;

		/*
			Passes catched events to signal listeners in prioritized order.
		*/
		class EventSignaler
		{
		public:
			EventSignaler(EventCatcher& eventCatcher);

			void update();
			
			void connectToKeyboardPressEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(KeyboardPressEvent&)>& callback, const int priority);
			void connectToKeyboardReleaseEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(KeyboardReleaseEvent&)>& callback, const int priority);
			void connectToMouseButtonPressEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseButtonPressEvent&)>& callback, const int priority);
			void connectToMouseButtonReleaseEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseButtonReleaseEvent&)>& callback, const int priority);
			void connectToMouseMotionEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseMotionEvent&)>& callback, const int priority);
			void connectToMouseWheelEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseWheelEvent&)>& callback, const int priority);
			void connectToMouseHoverEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(MouseHoverEvent&)>& callback, const int priority);
			//TODO: joysticks...

		private:
			template<typename T>
			struct PrioritizedEventSignal
			{
				boost::signal<void(T&)> signal;
				int priority = 0;
			};
			template<typename EventType>
			void connectToEventSignal(boost::signals::scoped_connection& scopedConnection, const boost::function<void(EventType&)>& callback, const int priority, std::vector<std::unique_ptr<PrioritizedEventSignal<EventType>>>& signals)
			{
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
					log::warning("Another source has already connected to this signal with the same priority");
					scopedConnection = (*it)->signal.connect(callback);
				}
			}

			EventCatcher& eventCatcher;
			std::vector<std::unique_ptr<PrioritizedEventSignal<KeyboardPressEvent>>> keyboardPressSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<KeyboardReleaseEvent>>> keyboardReleaseSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseButtonPressEvent>>> mouseButtonPressSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseButtonReleaseEvent>>> mouseButtonReleaseSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseMotionEvent>>> mouseMotionSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseWheelEvent>>> mouseWheelSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseHoverEvent>>> mouseHoverSignals;
			//std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickButtonPressEvent>>> joystickButtonPressSignals;
			//std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickButtonReleaseEvent>>> joystickButtonReleaseSignals;
			//std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickAxisEvent>>> joystickAxisSignals;
		};
	}
}