#pragma once

#include "stdafx.h"
#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"


namespace se::gui
{
	class Element;

#define GUI_DEFINE_EVENT(EVENT, PARAM_TYPE) \
	inline ::se::gui::Connection on##EVENT(const std::function<bool(const ::se::gui::PARAM_TYPE&)>& _func) \
	{ return onEvent<::se::gui::PARAM_TYPE>(#EVENT, _func); } \
	inline ::se::gui::Connection on##EVENT(const std::function<bool()>& _func) \
	{ return onEvent(#EVENT, _func); }

#define GUI_DEFINE_TUNNELING_EVENT(EVENT, PARAM_TYPE) \
	inline ::se::gui::Connection onPreview##EVENT(const std::function<bool(const ::se::gui::PARAM_TYPE&)>& _func) \
	{ return onPreviewEvent<::se::gui::PARAM_TYPE>(#EVENT, _func); } \
	inline ::se::gui::Connection onPreview##EVENT(const std::function<bool()>& _func) \
	{ return onPreviewEvent(#EVENT, _func); } \
	GUI_DEFINE_EVENT(EVENT, PARAM_TYPE)

#define GUI_DEFINE_MOUSEBUTTON_EVENT(BUTTON, TYPE) \
	::se::gui::Connection onMouse##BUTTON##TYPE(const std::function<bool(const ::se::gui::MouseButtonArgs&)>& _func) \
	{ \
		return onEvent<::se::gui::MouseButtonArgs>( \
			"MouseButton", \
			[_func](const ::se::gui::MouseButtonArgs& _args) -> bool \
			{ \
				if (_args.button == ::se::input::MouseButton::BUTTON && \
					_args.type == ::se::input::MouseButtonEvent::Type::TYPE) \
				{ \
					return _func(_args); \
				} \
				return false; \
			}); \
	} \
	::se::gui::Connection onMouse##BUTTON##TYPE(const std::function<bool()>& _func) \
	{ \
		return onEvent<::se::gui::MouseButtonArgs>( \
			"MouseButton", \
			[_func](const ::se::gui::MouseButtonArgs& _args) -> bool \
			{ \
				if (_args.button == ::se::input::MouseButton::BUTTON && \
					_args.type == ::se::input::MouseButtonEvent::Type::TYPE) \
				{ \
					return _func(); \
				} \
				return false; \
			}); \
	}

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
		Element* source;
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
		MouseHoverArgs(std::string_view _eventName, const input::MouseHoverEvent& _inputEvent)
			: MouseEventArgs(_eventName, Vec2(_inputEvent.position, UnitType::Pixels)) {}
	};
	struct MouseMotionArgs : MouseEventArgs
	{
		MouseMotionArgs(std::string_view _eventName, const input::MouseMotionEvent& _inputEvent)
			: MouseEventArgs(_eventName, Vec2(_inputEvent.position, UnitType::Pixels))
			, previousPosition(_inputEvent.previousPosition, UnitType::Pixels) {}
		const Vec2 previousPosition;
	};
	struct MouseWheelArgs : MouseEventArgs
	{
		MouseWheelArgs(std::string_view _eventName, const input::MouseWheelEvent& _inputEvent)
			: MouseEventArgs(_eventName, getMousePosition())
			, delta(_inputEvent.delta) {}
		const glm::ivec2& delta;
	};
	struct MouseButtonArgs : MouseEventArgs
	{
		MouseButtonArgs(std::string_view _eventName, const input::MouseButtonEvent& _inputEvent)
			: MouseEventArgs(_eventName, getMousePosition())
			, button(_inputEvent.button)
			, type(_inputEvent.type) {}
		const input::MouseButton& button;
		const input::MouseButtonEvent::Type& type;
	};


	class SE_INTERFACE IEventRouter
	{
	public:

		template<typename T>
		Connection onPreviewEvent(const std::string& _event, const std::function<bool(const T&)>& _func)
		{
			return previewEventHandlers[_event].connect(
				[_func](const EventArgs& _args) -> bool
				{
					try
					{
						return _func(dynamic_cast<const T&>(_args));
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

		template<typename T>
		Connection onEvent(const std::string& _event, const std::function<bool(const T&)>& _func)
		{
			return eventHandlers[_event].connect(
				[_func](const EventArgs& _args) -> bool
				{
					try
					{
						return _func(dynamic_cast<const T&>(_args));
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

		GUI_DEFINE_TUNNELING_EVENT(MouseHover,	MouseHoverArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseMotion, MouseMotionArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseWheel,	MouseWheelArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseButton, MouseButtonArgs)
		GUI_DEFINE_MOUSEBUTTON_EVENT(Right, Press)
		GUI_DEFINE_MOUSEBUTTON_EVENT(Left,	Press)
		GUI_DEFINE_MOUSEBUTTON_EVENT(Right, Release)
		GUI_DEFINE_MOUSEBUTTON_EVENT(Left,	Release)
			// TODO: MouseEnter, MouseLeave

	protected:

		enum class RouteResult
		{
			NotHandled,
			Bubbling,
			Handled,
		};

		using EventRoutingFn = std::function<RouteResult(IEventRouter*, const EventArgs&)>;
		virtual RouteResult routeChildren(const EventRoutingFn&, const EventArgs&)
		{
			return RouteResult::NotHandled;
		}
		virtual RouteResult routeParent(const EventRoutingFn&, const EventArgs&)
		{
			return RouteResult::NotHandled;
		}

		// Called for System and Tunneling events.
		// return true means Element wants to handle the event (EventArgs::source should be set)
		virtual bool handleRoutedEvent(const EventArgs&) = 0;

		RouteResult routeSystemEvent(const EventArgs& _args)
		{
			// System events do both tunneling and bubbling
			switch (routeChildren(&IEventRouter::routeSystemEvent, _args))
			{
				case RouteResult::NotHandled:
					if (handleRoutedEvent(_args))
						return raiseTunnelingEvent(_args);
					return RouteResult::NotHandled;
				case RouteResult::Bubbling:
					if (!handleRoutedEvent(_args))
						return RouteResult::Bubbling;
					callEventHandlers(_args);
					return RouteResult::Handled;
				case RouteResult::Handled:
					return RouteResult::Handled;
			}
			se_assert(false);
			return RouteResult::NotHandled;
		}
		RouteResult raiseTunnelingEvent(const EventArgs& _args)
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
		RouteResult raiseBubblingEvent(const EventArgs& _args)
		{
			if (callEventHandlers(_args))
				return RouteResult::Handled;
			return routeParent(&IEventRouter::raiseBubblingEvent, _args);
		}
		bool raiseDirectEvent(const EventArgs& _args)
		{
			return callEventHandlers(_args);
		}

	private:

		RouteResult tunnelRoutePreviewEvents(const EventArgs& _args)
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
		bool callEventHandlers(const EventArgs& _args)
		{
			if (eventHandlers.count(_args.eventName) == 0)
				return false;
			if (auto result = eventHandlers[_args.eventName](_args))
				return result.value();
			return false;
		}
		bool callPreviewEventHandlers(const EventArgs& _args)
		{
			if (previewEventHandlers.count(_args.eventName) == 0)
				return false;
			if (auto result = previewEventHandlers[_args.eventName](_args))
				return result.value();
			return false;
		}

		std::unordered_map<std::string, Signal<bool(const EventArgs&)>> previewEventHandlers;
		std::unordered_map<std::string, Signal<bool(const EventArgs&)>> eventHandlers;
	};
}