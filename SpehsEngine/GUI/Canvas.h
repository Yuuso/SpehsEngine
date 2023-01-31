#pragma once

#include "stdafx.h"
#include "SpehsEngine/Core/ScopedConnections.h"
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
			_eventSignaler.connectToMouseHoverSignal(
				connections.add(),
				[this](const input::MouseHoverEvent& _inputEvent)
				{
					auto args = MouseHoverArgs(_inputEvent);
					return routeSystemEvent(args) == RouteResult::Handled;
				}, _inputPriority);

			_eventSignaler.connectToMouseMotionSignal(
				connections.add(),
				[this](const input::MouseMotionEvent& _inputEvent)
				{
					auto args = MouseMotionArgs(_inputEvent);
					return routeSystemEvent(args) == RouteResult::Handled;
				}, _inputPriority);

			_eventSignaler.connectToMouseWheelSignal(
				connections.add(),
				[this](const input::MouseWheelEvent& _inputEvent)
				{
					auto args = MouseWheelArgs(_inputEvent);
					return routeSystemEvent(args) == RouteResult::Handled;
				}, _inputPriority);

			_eventSignaler.connectToMouseButtonSignal(
				connections.add(),
				[this](const input::MouseButtonEvent& _inputEvent)
				{
					auto args = MouseButtonArgs(_inputEvent);
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
		template<typename ElementType>
		std::shared_ptr<ElementType>& add()
		{
			static_assert(std::is_base_of<Element, ElementType>::value,
				"Canvas::add ElementType must inherit Element");
			rootElements.push_back(std::make_shared<ElementType>());
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
		ScopedConnections connections;

		std::vector<std::shared_ptr<Element>> rootElements;
		bool updating = false;
	};
}
