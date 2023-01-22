#pragma once

#include "stdafx.h" //
#include "SpehsEngine/GUI/Binding.h"
#include "SpehsEngine/GUI/PropertyHost.h"
#include "SpehsEngine/GUI/EventRouter.h"


namespace se::gui
{
	class Element : public IPropertyHost, public IEventRouter
	{
		GUI_PROPERTY_CLASS(Element)


		// <Example>
	private:
		static std::any property_ExampleGet(const IPropertyHost* _instance)
		{
			return static_cast<const SelfClass*>(_instance)->getExample();
		}
		static void property_ExampleSet(IPropertyHost* _instance, const std::any& _value)
		{
			try
			{
				static_cast<SelfClass*>(_instance)->setExample(std::any_cast<const int&>(_value));
			}
			catch (const std::bad_any_cast&)
			{
				se::log::warning("Invalid 'Example' property value type!");
			}
		}
		static bool property_ExampleCompare(IPropertyHost* _instance, const std::any& _value)
		{
			try
			{
				return static_cast<const SelfClass*>(_instance)->getExample() == std::any_cast<const int&>(_value);
			}
			catch (const std::bad_any_cast&)
			{
				se::log::warning("Invalid 'Example' property value type!");
			}
			return false;
		}
		static void property_ExampleSetTriggerValue(IPropertyHost* _instance, const std::any& _value)
		{
			static_cast<SelfClass*>(_instance)->setExampleTriggerValue(_value);
		}
		inline static const PropertyLink property_ExampleLink{property_Map, "Example",
			property_ExampleGet, property_ExampleSet, property_ExampleCompare, property_ExampleSetTriggerValue, 0};
		std::optional<int> property_Example;
		const int* property_ExampleTriggerValue = nullptr;
		bool property_ExampleSetterGuard = false;
	public:
		SelfClass& setExample(const int& _value)
		{
			if (property_ExampleSetterGuard)
			{
				// To prevent recursion
				return *this;
			}
			if (_value == property_Example)
			{
				return *this;
			}

			property_Example = _value;
			if (!property_ExampleTriggerValue)
			{
				property_ExampleSetterGuard = true;
				propertyChangedSignal("Example", _value);
				property_ExampleSetterGuard = false;
			}

			return *this;
		}
		SelfClass& setExample(const Binding& _binding)
		{
			eraseIf(bindings,
				[](const std::unique_ptr<BindingLink>& binding)
				{ return &binding->propertyLink == &property_ExampleLink; });

			bindings.push_back(
				std::make_unique<BindingLink>(
					bindingResolveDefaultValue(_binding, BindingMode::OneWay, BindingSourceUpdate::PropertyChanged),
					property_ExampleLink));
			resolveBinding(*bindings.back());
			return *this;
		}
		const int& getExample() const
		{
			// Local triggers
			if (property_ExampleTriggerValue)
			{
				return *property_ExampleTriggerValue;
			}

			// Local property value or Binding
			if (property_Example.has_value())
			{
				return property_Example.value();
			}

			// Style
			// TODO

			// Fallback
			return std::any_cast<const int&>(property_ExampleLink.defaultValue);
		}
	private:
		void setExampleTriggerValue(const std::any& _value)
		{
			se_assert(!property_ExampleSetterGuard);

			if (!_value.has_value())
			{
				if (!property_ExampleTriggerValue)
					return;
				property_ExampleTriggerValue = nullptr;
				property_ExampleSetterGuard = true;
				propertyChangedSignal("Example", getExample());
				property_ExampleSetterGuard = false;
				return;
			}

			try
			{
				property_ExampleTriggerValue = &std::any_cast<const int&>(_value);
			}
			catch (const std::bad_any_cast&)
			{
				se::log::warning("Invalid 'Example' property value type!");
			}
			property_ExampleSetterGuard = true;
			propertyChangedSignal("Example", _value);
			property_ExampleSetterGuard = false;
		}
		// </Example>

		// <ReadOnlyExample>
		// </ReadOnlyExample>


	public:
		void addTrigger(const Trigger& _trigger)
		{
			triggers.push_back(std::make_unique<TriggerLink>(_trigger));
			connectTrigger(*triggers.back());
		}


	public:
		void setDataContext(std::shared_ptr<IPropertyHost> _dataContext)
		{
			dataContext = _dataContext;
			onDataContextChanged();
		}
	protected:
		void setParent(Element* _parent)
		{
			if (parent != _parent)
			{
				parent = _parent;
				onParentChanged();
			}
		}
		virtual void onDataContextChanged()
		{
			for (auto&& bindingLink : bindings)
			{
				resolveBinding(*bindingLink);
			}
			for (auto&& triggerLink : triggers)
			{
				connectTrigger(*triggerLink);
			}
		}
		virtual void onParentChanged()
		{
			for (auto&& bindingLink : bindings)
			{
				resolveBinding(*bindingLink);
			}
			for (auto&& triggerLink : triggers)
			{
				connectTrigger(*triggerLink);
			}
		}

	protected:
		IPropertyHost* findBindingRelativeSource(const Binding& _binding, unsigned int& depth)
		{
			if (!_binding.sourceElementName.empty())
			{
				if (name == _binding.sourceElementName)
				{
					if (hasProperty(_binding.sourcePropertyName))
					{
						return this;
					}
					if (dataContext && dataContext->hasProperty(_binding.sourcePropertyName))
					{
						return dataContext.get();
					}
					se::log::warning("No property '" + _binding.sourcePropertyName + "' found in Element '" + name + "'!");
					return nullptr;
				}
			}
			else if (dataContext)
			{
				if (dataContext->hasProperty(_binding.sourcePropertyName))
				{
					return dataContext.get();
				}
				se::log::warning("No property '" + _binding.sourcePropertyName + "' found in DataContext!");
				return nullptr;
			}

			if (parent && 0u != depth--)
			{
				return parent->findBindingRelativeSource(_binding, depth);
			}
			return nullptr;
		}
		IPropertyHost* findBindingSource(const Binding& _binding)
		{
			if (_binding.relativeSource.mode == RelativeSourceMode::Self)
			{
				if (hasProperty(_binding.sourcePropertyName))
				{
					return this;
				}
				se::log::warning("No property '" + _binding.sourcePropertyName + "' found in Element '" + name + "'!");
				return nullptr;
			}

			se_assert(_binding.relativeSource.mode == RelativeSourceMode::Parent);
			unsigned int depth = _binding.relativeSource.parentDepth;
			return findBindingRelativeSource(_binding, depth);
		}
		void resolveBinding(BindingLink& _bindingLink)
		{
			_bindingLink.sourceChangedConnection.disconnect();
			_bindingLink.targetChangedConnection.disconnect();

			IPropertyHost* source = findBindingSource(_bindingLink.binding);
			if (!source)
			{
				if (bindingShouldUpdateTargetOnPropertyChanged(_bindingLink.binding))
				{
					_bindingLink.propertyLink.setProperty(this, _bindingLink.propertyLink.defaultValue);
				}
				return;
			}

			se_assert(source->hasProperty(_bindingLink.binding.sourcePropertyName));
			const std::any sourcePropertyValue = source->getProperty(_bindingLink.binding.sourcePropertyName);
			se_assert(sourcePropertyValue.has_value());

			if (sourcePropertyValue.type() != _bindingLink.propertyLink.defaultValue.type())
			{
				// TODO: Converter
				se::log::warning("Invalid binding '" + _bindingLink.binding.sourcePropertyName + "' source type!");
				if (bindingShouldUpdateTargetOnPropertyChanged(_bindingLink.binding))
				{
					_bindingLink.propertyLink.setProperty(this, _bindingLink.propertyLink.defaultValue);
				}
				return;
			}

			if (bindingShouldUpdateTargetInitial(_bindingLink.binding))
			{
				_bindingLink.propertyLink.setProperty(this, sourcePropertyValue);
			}

			if (bindingShouldUpdateTargetOnPropertyChanged(_bindingLink.binding))
			{
				_bindingLink.sourceChangedConnection = source->onPropertyChanged(
					[this, &_bindingLink](const std::string& _name, const std::any& _value)
					{
						if (_name == _bindingLink.binding.sourcePropertyName)
						{
							_bindingLink.propertyLink.setProperty(this, _value);
						}
					});
			}

			if (bindingShouldUpdateSourceOnPropertyChanged(_bindingLink.binding))
			{
				_bindingLink.targetChangedConnection = onPropertyChanged(
					[source, &_bindingLink](const std::string& _name, const std::any& _value)
					{
						if (_name == _bindingLink.propertyLink.name)
						{
							source->setProperty(_bindingLink.binding.sourcePropertyName, _value);
						}
					});
			}
		}
		void connectTrigger(TriggerLink& _triggerLink)
		{
			_triggerLink.sourceChangedConnection.disconnect();
			IPropertyHost* source = findBindingSource(_triggerLink.trigger.binding);
			if (!source)
			{
				return;
			}

			updateTriggerTargers(_triggerLink);

			_triggerLink.sourceChangedConnection = source->onPropertyChanged(
				[this, &_triggerLink](const std::string& _property, const std::any&)
				{
					if (_property == _triggerLink.trigger.binding.sourcePropertyName)
					{
						updateTriggerTargers(_triggerLink);
					}
				}
			);
		}
		void updateTriggerTargers(TriggerLink& _triggerLink)
		{
			const bool triggerActive = compareProperty(_triggerLink.trigger.binding.sourcePropertyName, _triggerLink.trigger.value);
			if (triggerActive == _triggerLink.active)
			{
				return;
			}
			_triggerLink.active = triggerActive;

			for (auto&& setter : _triggerLink.trigger.setters)
			{
				setPropertyTriggerValue(setter.propertyName, triggerActive ? setter.value : std::any{});
			}

			auto& actions = triggerActive
				? _triggerLink.trigger.enterActions
				: _triggerLink.trigger.exitActions;
			for (auto&& action : actions)
			{
				action->execute(this);
			}
		}

	public:
		GUI_DEFINE_EVENT(MouseEnter, MouseButtonArgs)
		GUI_DEFINE_EVENT(MouseLeave, MouseButtonArgs)
	protected:
		RouteResult routeParent(const EventRoutingFn& _func, EventArgs& _args) override
		{
			return parent ? _func(static_cast<IEventRouter*>(parent), _args) : RouteResult::NotHandled;
		}
		virtual bool handleRoutedEvent(EventArgs& _args) override
		{
			try
			{
				dynamic_cast<const MouseEventArgs&>(_args);
				// TODO: hit testing
				// TODO: MouseEnter, MouseLeave
				_args.source = this;
				return true;
			}
			catch (std::bad_cast){}
			return false;
		}


	public:
		const std::string& getName() const { return name; }

	protected:
		std::string name = "Test Element";
		Element* parent = nullptr;
		std::shared_ptr<IPropertyHost> dataContext;
		std::vector<std::unique_ptr<BindingLink>> bindings;
		std::vector<std::unique_ptr<TriggerLink>> triggers;
	};
}
