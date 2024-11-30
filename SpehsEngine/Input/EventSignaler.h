#pragma once

#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Input/CustomEventParameters.h"


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

			void signalEvents(EventCatcher& eventCatcher);

			void connectToPreUpdateSignal(ScopedConnection& scopedConnection, const std::function<void()>& callback) { scopedConnection = preUpdateSignal.connect(callback); }
			void connectToPostUpdateSignal(ScopedConnection& scopedConnection, const std::function<void()>& callback) { scopedConnection = postUpdateSignal.connect(callback); }
			void connectToMouseHoverSignal(ScopedConnection& scopedConnection, const std::function<bool(const MouseHoverEvent&)>& callback, const int priority);
			void connectToMouseMotionSignal(ScopedConnection& scopedConnection, const std::function<bool(const MouseMotionEvent&)>& callback, const int priority);
			void connectToMouseWheelSignal(ScopedConnection& scopedConnection, const std::function<bool(const MouseWheelEvent&)>& callback, const int priority);
			void connectToMouseButtonSignal(ScopedConnection& scopedConnection, const std::function<bool(const MouseButtonEvent&)>& callback, const int priority);
			void connectToKeyboardSignal(ScopedConnection& scopedConnection, const std::function<bool(const KeyboardEvent&)>& callback, const int priority);
			void connectToTextInputSignal(ScopedConnection& scopedConnection, const std::function<bool(const TextInputEvent&)>& callback, const int priority);
			void connectToJoystickButtonSignal(ScopedConnection& scopedConnection, const std::function<bool(const JoystickButtonEvent&)>& callback, const int priority);
			void connectToJoystickAxisSignal(ScopedConnection& scopedConnection, const std::function<bool(const JoystickAxisEvent&)>& callback, const int priority);
			void connectToJoystickHatSignal(ScopedConnection& scopedConnection, const std::function<bool(const JoystickHatEvent&)>& callback, const int priority);
			void connectToQuitSignal(ScopedConnection& scopedConnection, const std::function<bool(const QuitEvent&)>& callback, const int priority);
			void connectToFileDropSignal(ScopedConnection& scopedConnection, const std::function<bool(const FileDropEvent&)>& callback, const int priority);

			template<typename CustomEvent>
			void connectToCustomEventSignal(
				ScopedConnection& scopedConnection, const std::function<bool(const CustomEvent&)>& callback, const int priority)
			{
				static_assert(std::is_base_of<ICustomEvent, CustomEvent>::value);
				scopedConnection.disconnect();
				std::vector<std::unique_ptr<CustomSignal>>& customSignals = customEventSignalContainers[typeid(CustomEvent).hash_code()];
				const std::vector<std::unique_ptr<CustomSignal>>::iterator it = std::find_if(customSignals.begin(), customSignals.end(),
					[priority](const std::unique_ptr<CustomSignal>& signal)->bool { return priority == signal->priority; });
				if (it == customSignals.end())
				{
					customSignals.push_back(std::unique_ptr<CustomSignal>(new CustomSignal()));
					customSignals.back()->priority = priority;
					scopedConnection = customSignals.back()->signal.connect(
						[callback](const void* const eventPtr)->bool
						{
							return callback(*((const CustomEvent*)eventPtr));
						});
					std::sort(customSignals.begin(), customSignals.end(), [](const std::unique_ptr<CustomSignal>& _a, const std::unique_ptr<CustomSignal>& _b) { return _a->priority > _b->priority; });
				}
				else
				{
					if (priority != maxPriority && priority != minPriority && !(*it)->signal.isEmpty())
					{
						log::error("Another source has already connected to this signal with the same priority: " + std::to_string(priority));
					}
					scopedConnection = (*it)->signal.connect(
						[callback](const void* const eventPtr)->bool
						{
							return callback(*((const CustomEvent*)eventPtr));
						});
				}
			}

			// Custom signals are always generated from external sources
			template<typename CustomEvent>
			bool signalCustomEvent(const CustomEvent& customEvent)
			{
				static_assert(std::is_base_of<ICustomEvent, CustomEvent>::value);
				std::unordered_map<size_t, std::vector<std::unique_ptr<CustomSignal>>>::iterator it = customEventSignalContainers.find(typeid(CustomEvent).hash_code());
				if (it != customEventSignalContainers.end())
				{
					for (size_t s = 0; s < it->second.size(); s++)
					{
						if (it->second[s]->signal.isEmpty())
						{
							it->second.erase(it->second.begin() + s--);
						}
						else if (const std::optional<bool> consumed = it->second[s]->signal(&customEvent))
						{
							if (consumed.value())
							{
								//A connected receiver used the event, do not call receivers with a lower priority.
								return true;
							}
						}
					}
				}
				return false;
			}

		private:

			template<typename EventType>
			struct PrioritizedEventSignal
			{
				Signal<bool(const EventType&)> signal;
				int priority = 0;
			};

			struct CustomSignal
			{
				Signal<bool(const void* const)> signal;
				int priority = 0;
			};

			template<typename EventType>
			void connectToEventSignal(ScopedConnection& scopedConnection, const std::function<bool(const EventType&)>& callback, const int priority, std::vector<std::unique_ptr<PrioritizedEventSignal<EventType>>>& signals)
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
					if (priority != maxPriority && priority != minPriority && (*it)->signal.getConnectionCount() > 0)
					{
						log::error("Another source has already connected to this signal with the same priority: " + std::to_string(priority));
					}
					scopedConnection = (*it)->signal.connect(callback);
				}
			}

			Signal<void(void)> preUpdateSignal;
			Signal<void(void)> postUpdateSignal;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseHoverEvent>>> mouseHoverSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseMotionEvent>>> mouseMotionSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseWheelEvent>>> mouseWheelSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<MouseButtonEvent>>> mouseButtonSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<KeyboardEvent>>> keyboardSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<TextInputEvent>>> textInputSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickButtonEvent>>> joystickButtonSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickAxisEvent>>> joystickAxisSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<JoystickHatEvent>>> joystickHatSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<QuitEvent>>> quitSignals;
			std::vector<std::unique_ptr<PrioritizedEventSignal<FileDropEvent>>> fileDropSignals;
			std::unordered_map<size_t/*typeid hash code*/, std::vector<std::unique_ptr<CustomSignal>>> customEventSignalContainers;
		};
	}
}

/*
	Copy & paste cheatsheet
	For any class wishing to listen for any/all of the following events.

	//.h
	#include "SpehsEngine/Input/Event.h"

	void eventSignalerPreUpdateCallback();
	void eventSignalerPostUpdateCallback();
	bool mouseHoverCallback(const se::input::MouseHoverEvent& event);
	bool mouseMotionCallback(const se::input::MouseMotionEvent& event);
	bool mouseWheelCallback(const se::input::MouseWheelEvent& event);
	bool mouseButtonCallback(const se::input::MouseButtonEvent& event);
	bool keyboardCallback(const se::input::KeyboardEvent& event);
	bool textInputCallback(const se::input::TextInputEvent& event);

	ScopedConnection eventSignalerPreUpdateConnection;
	ScopedConnection eventSignalerPostUpdateConnection;
	ScopedConnection mouseHoverConnection;
	ScopedConnection mouseMotionConnection;
	ScopedConnection mouseWheelConnection;
	ScopedConnection mouseButtonConnection;
	ScopedConnection keyboardConnection;
	ScopedConnection textInputConnection;

	//.cpp
	#include "SpehsEngine/Input/EventSignaler.h"
	const int inputPriority = INPUT_PRIORITY;
	eventSignaler.connectToPreUpdateSignal(eventSignalerPreUpdateConnection, std::bind(&MyClass::eventSignalerPreUpdateCallback, this));
	eventSignaler.connectToPostUpdateSignal(eventSignalerPostUpdateConnection, std::bind(&MyClass::eventSignalerPostUpdateCallback, this));
	eventSignaler.connectToMouseHoverSignal(mouseWheelConnection, std::bind(&MyClass::mouseHoverCallback, this, std::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseMotionSignal(mouseMotionConnection, std::bind(&MyClass::mouseMotionCallback, this, std::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseWheelSignal(mouseWheelConnection, std::bind(&MyClass::mouseWheelCallback, this, std::placeholders::_1), inputPriority);
	eventSignaler.connectToMouseButtonSignal(mouseButtonConnection, std::bind(&MyClass::mouseButtonCallback, this, std::placeholders::_1), inputPriority);
	eventSignaler.connectToKeyboardSignal(keyboardConnection, std::bind(&MyClass::keyboardCallback, this, std::placeholders::_1), inputPriority);
	eventSignaler.connectToTextInputSignal(textInputConnection, std::bind(&MyClass::textInputCallback, this, std::placeholders::_1), inputPriority);

	void MyClass::eventSignalerPreUpdateCallback()
	{

	}

	void MyClass::eventSignalerPostUpdateCallback()
	{

	}

	bool MyClass::mouseHoverCallback(const se::input::MouseHoverEvent& event)
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

	bool MyClass::mouseButtonCallback(const se::input::MouseButtonEvent& event)
	{
		return false;
	}

	bool MyClass::keyboardCallback(const se::input::KeyboardEvent& event)
	{
		return false;
	}

	bool MyClass::textInputCallback(const se::input::TextInputEvent& event)
	{
		return false;
	}
*/
