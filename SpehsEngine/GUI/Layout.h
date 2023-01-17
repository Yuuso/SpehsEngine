#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/Element.h"
#include "SpehsEngine/GUI/EventRouter.h"
#include "SpehsEngine/Input/EventSignaler.h"


namespace se::gui
{
	class Layout : public IEventRouter
	{
	public:

		SE_NO_COPY(Layout)

		Layout(input::EventSignaler& _eventSignaler, int _inputPriority)
			: eventSignaler(_eventSignaler)
		{
			inputConnections.reserve(6);

			_eventSignaler.connectToMouseHoverSignal(
				inputConnections.emplace_back(),
				[this](const input::MouseHoverEvent& _inputEvent)
				{
					return routeEventTunnel(MouseHoverArgs("MouseHover", _inputEvent));
				}, _inputPriority);

			//_eventSignaler.connectToMouseMotionSignal(mouseMotionConnection, [this](const input::MouseButtonEvent& _event){ return mouseButtonCallback(_event); }, _inputPriority);
			//_eventSignaler.connectToMouseWheelSignal(mouseWheelConnection, [this](const input::MouseButtonEvent& _event){ return mouseButtonCallback(_event); }, _inputPriority);
			//_eventSignaler.connectToMouseButtonSignal(mouseButtonConnection, [this](const input::MouseButtonEvent& _event){ return mouseButtonCallback(_event); }, _inputPriority);
			//_eventSignaler.connectToKeyboardSignal(keyboardConnection, [this](const input::MouseButtonEvent& _event){ return mouseButtonCallback(_event); }, _inputPriority);
			//_eventSignaler.connectToTextInputSignal(textInputConnection, [this](const input::MouseButtonEvent& _event){ return mouseButtonCallback(_event); }, _inputPriority);
		}

		void update()
		{
			// deinit
			// init
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
			static_assert(std::is_base_of<Element, T>::value, "Layout::add T must inherit Element");
			rootElements.push_back(std::make_shared<T>());
			return rootElements.back();
		}

	private:

		bool routeChildren(const EventRoutingFunction& _func, const EventArgs& _args) override
		{
			for (auto&& element : rootElements)
			{
				if (_func(static_cast<IEventRouter*>(element.get()), _args))
					return true;
			}
			return false;
		}

		input::EventSignaler& eventSignaler;
		std::vector<boost::signals2::scoped_connection> inputConnections;

		std::vector<std::shared_ptr<Element>> rootElements;
		bool updating = false;
	};
}
