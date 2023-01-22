#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/Element.h"
#include "SpehsEngine/GUI/EventRouter.h"
#include "SpehsEngine/Input/EventSignaler.h"


namespace se::gui
{
	class Canvas : public IEventRouter
	{
	public:

		SE_NO_COPY(Canvas)

		Canvas(input::EventSignaler& _eventSignaler, int _inputPriority)
			: eventSignaler(_eventSignaler)
		{
			inputConnections.reserve(4);

			_eventSignaler.connectToMouseHoverSignal(
				inputConnections.emplace_back(),
				[this](const input::MouseHoverEvent& _inputEvent)
				{
					auto args = MouseHoverArgs("MouseHover", _inputEvent);
					return routeSystemEvent(args) == RouteResult::Handled;
				}, _inputPriority);

			_eventSignaler.connectToMouseMotionSignal(
				inputConnections.emplace_back(),
				[this](const input::MouseMotionEvent& _inputEvent)
				{
					auto args = MouseMotionArgs("MouseMotion", _inputEvent);
					return routeSystemEvent(args) == RouteResult::Handled;
				}, _inputPriority);

			_eventSignaler.connectToMouseWheelSignal(
				inputConnections.emplace_back(),
				[this](const input::MouseWheelEvent& _inputEvent)
				{
					auto args = MouseWheelArgs("MouseWheel", _inputEvent);
					return routeSystemEvent(args) == RouteResult::Handled;
				}, _inputPriority);

			_eventSignaler.connectToMouseButtonSignal(
				inputConnections.emplace_back(),
				[this](const input::MouseButtonEvent& _inputEvent)
				{
					auto args = MouseButtonArgs("MouseButton", _inputEvent);
					return routeSystemEvent(args) == RouteResult::Handled;
				}, _inputPriority);
		}

		void update()
		{
			/// Connect to view prerender...
			// deinit removed elements
			// init new elements
			// update
		}
		void add(std::shared_ptr<Element>& _element)
		{
			se_assert(!updating);
			rootElements.push_back(_element);
		}
		template<typename T>
		std::shared_ptr<T>& add()
		{
			static_assert(std::is_base_of<Element, T>::value, "Canvas::add T must inherit Element");
			rootElements.push_back(std::make_shared<T>());
			return rootElements.back();
		}

	private:

		RouteResult routeChildren(const EventRoutingFn& _func, EventArgs& _args) override
		{
			RouteResult result = RouteResult::NotHandled;
			for (auto&& element : rootElements)
			{
				result = _func(static_cast<IEventRouter*>(element.get()), _args);
				if (result != RouteResult::NotHandled)
					return result;
			}
			return result;
		}
		bool handleRoutedEvent(EventArgs&) override
		{
			// Root element always handles all routed events
			return true;
		}

		input::EventSignaler& eventSignaler;
		std::vector<boost::signals2::scoped_connection> inputConnections;

		std::vector<std::shared_ptr<Element>> rootElements;
		bool updating = false;
	};
}
