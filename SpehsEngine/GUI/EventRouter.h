#pragma once

#include "stdafx.h"
#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"
#include "SpehsEngine/GUI/EventTrigger.h"


namespace se::gui
{
	class Element;

#define GUI_DEFINE_EVENT(EVENT, PARAM_TYPE) \
public: \
	inline ::se::Connection on##EVENT(const std::function<bool(const ::se::gui::PARAM_TYPE&)>& _func) \
	{ return onEvent<::se::gui::PARAM_TYPE>(#EVENT, _func); } \
	inline ::se::Connection on##EVENT(const std::function<bool()>& _func) \
	{ return onEvent(#EVENT, _func); }

#define GUI_DEFINE_TUNNELING_EVENT(EVENT, PARAM_TYPE) \
public: \
	inline ::se::Connection onPreview##EVENT(const std::function<bool(const ::se::gui::PARAM_TYPE&)>& _func) \
	{ return onPreviewEvent<::se::gui::PARAM_TYPE>(#EVENT, _func); } \
	inline ::se::Connection onPreview##EVENT(const std::function<bool()>& _func) \
	{ return onPreviewEvent(#EVENT, _func); } \
	GUI_DEFINE_EVENT(EVENT, PARAM_TYPE)

	// TODO: offset pixels to view

	static Vec2 getMousePosition()
	{
		std::optional<glm::vec2> pos = se::input::getMousePositionf();
		if (pos.has_value())
			return Vec2(pos.value(), UnitType::Pixels);
		return Vec2({ -1.0f, -1.0f }, UnitType::Auto);
	}

	struct EventArgs
	{
		EventArgs(std::string_view _eventName)
			: eventName(_eventName) {}
		virtual ~EventArgs() = default;
		const std::string eventName;
		Element* source = nullptr;
	};
	struct MouseEventArgs : EventArgs
	{
		MouseEventArgs(std::string_view _eventName, const Vec2&& _position)
			: EventArgs(_eventName)
			, position(_position) {}
		virtual ~MouseEventArgs() = default;
		const Vec2 position;
	};
	struct MouseHoverArgs : MouseEventArgs
	{
		MouseHoverArgs(const input::MouseHoverEvent& _inputEvent)
			: MouseEventArgs("MouseHover", Vec2(_inputEvent.position, UnitType::Pixels)) {}
	};
	struct MouseMotionArgs : MouseEventArgs
	{
		MouseMotionArgs(const input::MouseMotionEvent& _inputEvent)
			: MouseEventArgs("MouseMotion", Vec2(_inputEvent.position, UnitType::Pixels))
			, previousPosition(_inputEvent.previousPosition, UnitType::Pixels) {}
		const Vec2 previousPosition;
	};
	struct MouseWheelArgs : MouseEventArgs
	{
		MouseWheelArgs(const input::MouseWheelEvent& _inputEvent)
			: MouseEventArgs("MouseWheel", getMousePosition())
			, delta(_inputEvent.delta) {}
		const glm::ivec2& delta;
	};
	struct MouseButtonArgs : MouseEventArgs
	{
		MouseButtonArgs(const input::MouseButtonEvent& _inputEvent)
			: MouseEventArgs("MouseButton", getMousePosition())
			, button(_inputEvent.button)
			, type(_inputEvent.type) {}
		const input::MouseButton& button;
		const input::MouseButtonEvent::Type& type;
	};


	class SE_INTERFACE IEventRouter
	{
	public:

		template<typename EventArgsType>
		Connection onPreviewEvent(const std::string& _event, const std::function<bool(const EventArgsType&)>& _func)
		{
			static_assert(std::is_base_of<EventArgs, EventArgsType>::value, "onPreviewEvent EventArgsType must inherit EventArgs");
			return previewEventHandlers[_event].connect(
				[_func](const EventArgs& _args) -> bool
				{
					try
					{
						return _func(dynamic_cast<const EventArgsType&>(_args));
					}
					catch (std::bad_cast){}
					log::error("Invalid event args type!");
					return false;
				});
		}
		Connection onPreviewEvent(const std::string& _event, const std::function<bool()>& _func)
		{
			return previewEventHandlers[_event].connect([_func](auto&) -> bool { return _func(); });
		}

		template<typename EventArgsType>
		Connection onEvent(const std::string& _event, const std::function<bool(const EventArgsType&)>& _func)
		{
			static_assert(std::is_base_of<EventArgs, EventArgsType>::value, "onEvent EventArgsType must inherit EventArgs");
			return eventHandlers[_event].connect(
				[_func](const EventArgs& _args) -> bool
				{
					try
					{
						return _func(dynamic_cast<const EventArgsType&>(_args));
					}
					catch (std::bad_cast){}
					log::error("Invalid event args type!");
					return false;
				});
		}
		Connection onEvent(const std::string& _event, const std::function<bool()>& _func)
		{
			return eventHandlers[_event].connect([_func](auto&) -> bool { return _func(); });
		}
		Connection addEventTrigger(const EventTrigger& _eventTrigger)
		{
			return onEvent<EventArgs>(
				_eventTrigger.eventName,
				[_eventTrigger](const EventArgs& _args) -> bool
				{
					for (auto&& action : _eventTrigger.actions)
					{
						action->execute(_args.source);
					}
					return true;
				});
		}

		GUI_DEFINE_TUNNELING_EVENT(MouseHover,	MouseHoverArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseMotion, MouseMotionArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseWheel,	MouseWheelArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseButton, MouseButtonArgs)
		GUI_DEFINE_EVENT(MouseRightPress,	MouseButtonArgs)
		GUI_DEFINE_EVENT(MouseLeftPress,	MouseButtonArgs)
		GUI_DEFINE_EVENT(MouseRightRelease, MouseButtonArgs)
		GUI_DEFINE_EVENT(MouseLeftRelease,	MouseButtonArgs)

	protected:

		enum class RouteResult
		{
			NotHandled,
			Bubbling,
			// TODO: Handled doesn't always mean consumed!
			Handled,
		};

		using EventRoutingFn = std::function<RouteResult(IEventRouter*, EventArgs&)>;
		virtual RouteResult routeChildren(const EventRoutingFn&, EventArgs&)
		{
			return RouteResult::NotHandled;
		}
		virtual RouteResult routeParent(const EventRoutingFn&, EventArgs&)
		{
			return RouteResult::NotHandled;
		}

		// Called for System and Tunneling events.
		// return true means Element wants to handle the event (EventArgs::source should be set)
		virtual bool handleRoutedEvent(EventArgs&) = 0;

		RouteResult routeSystemEvent(EventArgs& _args)
		{
			// System events do both tunneling and bubbling
			switch (routeChildren(&IEventRouter::routeSystemEvent, _args))
			{
				case RouteResult::NotHandled:
					if (handleRoutedEvent(_args))
					{
						return raiseTunnelingEvent(_args);
					}
					return RouteResult::NotHandled;

				case RouteResult::Bubbling:
					if (handleRoutedEvent(_args))
					{
						callEventHandlers(_args);
						return RouteResult::Handled;
					}
					return RouteResult::Bubbling;

				case RouteResult::Handled:
					return RouteResult::Handled;
			}
			se_assert(false);
			return RouteResult::NotHandled;
		}
		RouteResult raiseTunnelingEvent(EventArgs& _args)
		{
			switch (tunnelRoutePreviewEvents(_args))
			{
				case RouteResult::Bubbling:
					se_assert(false);
					[[fallthrough]];

				case RouteResult::NotHandled:
					if (handleRoutedEvent(_args))
					{
						callEventHandlers(_args);
						return RouteResult::Handled;
					}
					return RouteResult::Bubbling;

				case RouteResult::Handled:
					return RouteResult::Handled;
			}
			se_assert(false);
			return RouteResult::NotHandled;
		}
		RouteResult raiseBubblingEvent(EventArgs& _args)
		{
			if (callEventHandlers(_args))
			{
				return RouteResult::Handled;
			}
			// TODO: Parent's should get Element.Event
			return routeParent(&IEventRouter::raiseBubblingEvent, _args);
		}
		bool raiseDirectEvent(const EventArgs& _args)
		{
			return callEventHandlers(_args);
		}

	private:

		RouteResult tunnelRoutePreviewEvents(EventArgs& _args)
		{
			switch (routeParent(&IEventRouter::tunnelRoutePreviewEvents, _args))
			{
				case RouteResult::Bubbling:
					se_assert(false);
					[[fallthrough]];

				case RouteResult::NotHandled:
					return callPreviewEventHandlers(_args) ? RouteResult::Handled : RouteResult::NotHandled;

				case RouteResult::Handled:
					return RouteResult::Handled;
			}
			se_assert(false);
			return RouteResult::NotHandled;
		}
		bool raiseAliasEvents(const EventArgs& _args)
		{
			if (_args.eventName == "MouseButton")
			{
				const MouseButtonArgs& mouseButtonArgs = static_cast<const MouseButtonArgs&>(_args);
			#define RAISE_ALIAS_EVENT(BUTTON, TYPE) \
				if (mouseButtonArgs.button == ::se::input::MouseButton::BUTTON && \
					mouseButtonArgs.type == ::se::input::MouseButtonEvent::Type::TYPE) \
				{ if (callAliasHandlers("Mouse"#BUTTON#TYPE, _args)) return true; }
				RAISE_ALIAS_EVENT(Right, Press)
				RAISE_ALIAS_EVENT(Left,	Press)
				RAISE_ALIAS_EVENT(Right, Release)
				RAISE_ALIAS_EVENT(Left,	Release)
			}
			return false;
		}
		bool callAliasHandlers(const std::string& _eventName, const EventArgs& _args)
		{
			if (eventHandlers.count(_eventName) == 0)
			{
				return false;
			}
			if (auto result = eventHandlers[_eventName](_args))
			{
				return result.value();
			}
			return false;
		}
		bool callEventHandlers(const EventArgs& _args)
		{
			if (callAliasHandlers(_args.eventName, _args))
			{
				return true;
			}
			return raiseAliasEvents(_args);
		}
		bool callPreviewEventHandlers(const EventArgs& _args)
		{
			if (previewEventHandlers.count(_args.eventName) == 0)
			{
				return false;
			}
			if (auto result = previewEventHandlers[_args.eventName](_args))
			{
				return result.value();
			}
			return false;
		}

		std::unordered_map<std::string, Signal<bool(const EventArgs&)>> previewEventHandlers;
		std::unordered_map<std::string, Signal<bool(const EventArgs&)>> eventHandlers;
	};
}