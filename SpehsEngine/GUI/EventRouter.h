#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/Element.h"
#include "SpehsEngine/Input/Event.h"


namespace se::gui
{
#define GUI_DEFINE_EVENT(EVENT, PARAM_TYPE) \
	inline ::se::gui::Connection on##EVENT(const std::function<bool(const PARAM_TYPE&)>& _func) \
	{ return onEvent<PARAM_TYPE>(#EVENT, _func); } \
	inline ::se::gui::Connection on##EVENT(const std::function<bool()>& _func) \
	{ return onEvent(#EVENT, _func); }
#define GUI_DEFINE_TUNNELING_EVENT(EVENT, PARAM_TYPE) \
	inline ::se::gui::Connection onPreview##EVENT(const std::function<bool(const PARAM_TYPE&)>& _func) \
	{ return onPreviewEvent<PARAM_TYPE>(#EVENT, _func); } \
	inline ::se::gui::Connection onPreview##EVENT(const std::function<bool()>& _func) \
	{ return onPreviewEvent(#EVENT, _func); } \
	GUI_DEFINE_EVENT(EVENT, PARAM_TYPE)

	//
	// MouseHover
	// MouseMotion -> MouseEnter, MouseLeave
	// MouseWheel
	// MouseButton -> MouseUp/Down/Repeat -> MouseLeft/Right/Middle/X1/X2Up
	// Keyboard -> KeyUp/Down/Repeat

	struct EventArgs
	{
		EventArgs(std::string_view _eventName)
			: eventName(_eventName) {}
		virtual ~EventArgs() = default;
		const std::string eventName;
		//Element* source = nullptr;
	};
	struct MouseHoverArgs : EventArgs
	{
		MouseHoverArgs(std::string_view _eventName, const input::MouseHoverEvent& _inputEvent)
			: EventArgs(_eventName), position(_inputEvent.position, UnitType::Pixels) {}
		const Vec2 position;
	};
	struct MouseMotionArgs : EventArgs
	{
		const Vec2 position;
		const Vec2 previousPosition;
		const Vec2 translation;
	};
	struct MouseWheelArgs : EventArgs
	{
		const input::MouseWheelEvent& inputEvent;
	};
	struct MouseButtonArgs : EventArgs
	{
		const input::MouseButtonEvent& inputEvent;
	};
	struct KeyboardArgs : EventArgs
	{
		const input::KeyboardEvent& inputEvent;
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

		GUI_DEFINE_TUNNELING_EVENT(MouseHover, MouseHoverArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseMotion, MouseMotionArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseWheel, MouseWheelArgs)
		GUI_DEFINE_TUNNELING_EVENT(MouseButton, MouseButtonArgs)
		GUI_DEFINE_TUNNELING_EVENT(Keyboard, KeyboardArgs)

	protected:

		using EventRoutingFunction = std::function<bool(IEventRouter*, const EventArgs&)>;
		virtual bool routeChildren(const EventRoutingFunction&, const EventArgs&)
		{
			return false;
		}
		virtual bool routeParent(const EventRoutingFunction&, const EventArgs&)
		{
			return false;
		}

		bool callEventHandlers(const EventArgs& _args)
		{
			if (eventHandlers.count(_args.eventName) == 0)
				return false;
			if (auto result = eventHandlers[_args.eventName](_args))
				return result.value();
			return false;
		}
		bool routeEventTunnel(const EventArgs& _args)
		{
			if (callPreviewEventHandlers(_args))
				return true;
			if (routeChildren(&IEventRouter::routeEventTunnel, _args))
				return true;
			if (callEventHandlers(_args))
				return true;
			return false;
		}
		bool routeEventBubble(const EventArgs& _args)
		{
			if (callEventHandlers(_args))
				return true;
			if (routeParent(&IEventRouter::routeEventBubble, _args))
				return true;
			return false;
		}

	private:

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