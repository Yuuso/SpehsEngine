#pragma once

#include "stdafx.h" //
#include "SpehsEngine/GUI/Binding.h"
#include "SpehsEngine/GUI/PropertyHost.h"
#include "SpehsEngine/GUI/PropertyLink.h"
#include "SpehsEngine/GUI/PropertyHostMeta.h"
#include "SpehsEngine/GUI/EventRouter.h"
#include "SpehsEngine/GUI/Resource.h"


namespace se::gui
{
	class Element : public IPropertyHost, public IEventRouter
	{
		GUI_REFLECT(Element, IPropertyHost)
		{
			GUI_REGISTER_MEMBER_PROPERTY(Example)
		}


	#pragma region Example
	private:
		inline static constexpr int property_ExampleDefault = 0;
		MemberProperty<int> property_Example{ property_ExampleDefault };
	public:
		SelfClass& setExample(const int& _value)
		{
			if (property_Example.setValue(_value, PropertyValueType::Local))
			{
				propertyChanged("Example",
					static_cast<const void*>(&property_Example.getValue()));
			}
			return *this;
		}
		SelfClass& setExample(const Binding& _binding)
		{
			setBinding(
				bindingResolveDefaultValue(
					_binding,
					BindingMode::OneWay,
					BindingSourceUpdate::PropertyChanged),
				*getPropertyLink("Example"),
				PropertyValueType::Local);
			return *this;
		}
		SelfClass& setExample(const Resource& /*_resource*/)
		{
			//unresolvedResources.push_back(
			//	UnresolvedResource(_resource,
			//	[this](const IResource* resource)
			//	{
			//	}));
			// if initialized, resolve immediately
			return *this;
		}
		const int& getExample() const
		{
			return property_Example.getValue();
		}
	#pragma endregion


	public:
		void addPropertyTrigger(const PropertyTrigger& _trigger)
		{
			propertyTriggers.push_back(std::make_unique<PropertyTriggerLink>(_trigger, PropertyValueType::Local));
			connectPropertyTrigger(*propertyTriggers.back());
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
				resolveBinding(*bindingLink, PropertyValueType::Local);
			}
			for (auto&& triggerLink : propertyTriggers)
			{
				connectPropertyTrigger(*triggerLink);
			}
		}
		virtual void onParentChanged()
		{
			for (auto&& bindingLink : bindings)
			{
				resolveBinding(*bindingLink, PropertyValueType::Local);
			}
			for (auto&& triggerLink : propertyTriggers)
			{
				connectPropertyTrigger(*triggerLink);
			}
		}
		void onInit()
		{
			// resouce
			raiseDirectEvent(EventArgs("Init"));
		}

	protected:
		void setBinding(const Binding& _binding, const IPropertyLink& _link, PropertyValueType _type)
		{
			eraseIf(bindings,
				[&_link](const std::unique_ptr<BindingLink>& binding)
				{ return &binding->propertyLink == &_link; });

			bindings.push_back(std::make_unique<BindingLink>(_binding, _link));
			resolveBinding(*bindings.back(), _type);
		}
		IPropertyHost* findBindingRelativeSource(const Binding& _binding, unsigned int& depth)
		{
			if (!_binding.sourceElementName.empty())
			{
				if (name == _binding.sourceElementName)
				{
					if (getPropertyLink(_binding.sourcePropertyName))
					{
						return this;
					}
					if (dataContext && dataContext->getPropertyLink(_binding.sourcePropertyName))
					{
						return dataContext.get();
					}
					se::log::warning("No property '" + _binding.sourcePropertyName + "' found in " + getSelfClassName() + " '" + name + "'!");
					return nullptr;
				}
			}
			else if (dataContext)
			{
				if (dataContext->getPropertyLink(_binding.sourcePropertyName))
				{
					return dataContext.get();
				}
				se::log::warning("No property '" + _binding.sourcePropertyName + "' found in " + getSelfClassName() + " '" + name + "' DataContext!");
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
				if (getPropertyLink(_binding.sourcePropertyName))
				{
					return this;
				}
				se::log::warning("No property '" + _binding.sourcePropertyName + "' found in " + getSelfClassName() + "!");
				return nullptr;
			}

			se_assert(_binding.relativeSource.mode == RelativeSourceMode::Parent);
			unsigned int depth = _binding.relativeSource.parentDepth;
			return findBindingRelativeSource(_binding, depth);
		}
		void resolveBinding(BindingLink& _bindingLink, PropertyValueType _type)
		{
			_bindingLink.sourceChangedConnection.disconnect();
			_bindingLink.targetChangedConnection.disconnect();

			IPropertyHost* source = findBindingSource(_bindingLink.binding);
			if (!source)
			{
				if (bindingShouldUpdateTargetOnPropertyChanged(_bindingLink.binding))
				{
					_bindingLink.propertyLink.resetValue(*this, _type);
				}
				return;
			}

			const IPropertyLink* sourcePropLink = source->getPropertyLink(_bindingLink.binding.sourcePropertyName);
			se_assert(sourcePropLink);

			if (sourcePropLink->getType() != _bindingLink.propertyLink.getType())
			{
				// TODO: Converter
				se::log::warning("Invalid binding '" + _bindingLink.binding.sourcePropertyName + "' source type!");
				if (bindingShouldUpdateTargetOnPropertyChanged(_bindingLink.binding))
				{
					_bindingLink.propertyLink.resetValue(*this, _type);
				}
				return;
			}

			if (bindingShouldUpdateTargetInitial(_bindingLink.binding))
			{
				_bindingLink.propertyLink.setValue(*this, sourcePropLink->getValue(*source), _type);
			}

			if (bindingShouldUpdateSourceInitial(_bindingLink.binding))
			{
				sourcePropLink->setValue(*source, _bindingLink.propertyLink.getValue(*this), PropertyValueType::Local);
			}

			if (bindingShouldUpdateTargetOnPropertyChanged(_bindingLink.binding))
			{
				_bindingLink.sourceChangedConnection = source->onPropertyChanged(
					[this, &_bindingLink, _type](const std::string& _name, const void* _value)
					{
						if (_name == _bindingLink.binding.sourcePropertyName)
						{
							ConnectionBlock recursionGuard{ _bindingLink.targetChangedConnection };
							_bindingLink.propertyLink.setValue(*this, _value, _type);
						}
					});
			}

			if (bindingShouldUpdateSourceOnPropertyChanged(_bindingLink.binding))
			{
				_bindingLink.targetChangedConnection = onPropertyChanged(
					[source, sourcePropLink, &_bindingLink, _type](const std::string& _name, const void* _value)
					{
						if (_name == _bindingLink.propertyLink.getName())
						{
							ConnectionBlock recursionGuard{ _bindingLink.sourceChangedConnection };
							sourcePropLink->setValue(*source, _value, PropertyValueType::Local);
						}
					});
			}
		}
		void connectPropertyTrigger(PropertyTriggerLink& _triggerLink)
		{
			_triggerLink.sourceChangedConnection.disconnect();
			_triggerLink.source = findBindingSource(_triggerLink.trigger.binding);
			if (!_triggerLink.source)
			{
				return;
			}

			const IPropertyLink* sourceLink = _triggerLink.source->getPropertyLink(_triggerLink.trigger.binding.sourcePropertyName);
			if (sourceLink->getType() != _triggerLink.trigger.value.type())
			{
				_triggerLink.source = nullptr;
				log::warning("Invalid PropertyTrigger value type! (" +
					_triggerLink.trigger.binding.sourcePropertyName + " != " + _triggerLink.trigger.value.type().name() + ")");
				return;
			}

			updatePropertyTriggerTargers(_triggerLink);

			_triggerLink.sourceChangedConnection = _triggerLink.source->onPropertyChanged(
				[this, &_triggerLink](const std::string& _name, const void*)
				{
					if (_name == _triggerLink.trigger.binding.sourcePropertyName)
					{
						updatePropertyTriggerTargers(_triggerLink);
					}
				});
		}
		void updatePropertyTriggerTargers(PropertyTriggerLink& _triggerLink)
		{
			const IPropertyLink* sourceLink = _triggerLink.source->getPropertyLink(_triggerLink.trigger.binding.sourcePropertyName);
			const bool triggerActive = sourceLink->compareValue(*_triggerLink.source, _triggerLink.trigger.value);
			if (triggerActive == _triggerLink.active)
			{
				return;
			}
			_triggerLink.active = triggerActive;

			for (auto&& setter : _triggerLink.trigger.setters)
			{
				const IPropertyLink* targetLink = getPropertyLink(setter.propertyName);
				if (!targetLink)
				{
					se::log::warning("No property '" + setter.propertyName + "' found in " + getSelfClassName() + "!");
					continue;
				}
				targetLink->setTriggerValue(*this, triggerActive ? setter.value : std::any{}, _triggerLink.type);
			}

			auto& actions = triggerActive
				? _triggerLink.trigger.enterActions
				: _triggerLink.trigger.exitActions;
			for (auto&& action : actions)
			{
				action->execute(this);
			}
		}

		void applyStyle(const Style& /*_style*/)
		{
			// todo
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
		std::vector<std::shared_ptr<const IResource>> resources;
		std::vector<UnresolvedResource> unresolvedResources;
		std::shared_ptr<IPropertyHost> dataContext;
		unique_ptr_vector<BindingLink> bindings;
		unique_ptr_vector<PropertyTriggerLink> propertyTriggers;
	};
}
