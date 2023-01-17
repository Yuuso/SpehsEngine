#pragma once

#include "stdafx.h" //
#include <any>
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
		inline static const PropertyLink property_ExampleLink{property_Map, "Example", property_ExampleGet, property_ExampleSet, 0};
		std::optional<int> property_Example;
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
			property_ExampleSetterGuard = true;

			property_Example = _value;
			propertyChangedSignal("Example", _value);

			property_ExampleSetterGuard = false;
			return *this;
		}
		SelfClass& setExample(const Binding& _binding)
		{
			auto it = std::find_if(
				bindings.begin(), bindings.end(),
				[](const std::unique_ptr<BindingLink>& binding)
				{ return &binding->propertyLink == &property_ExampleLink; });
			if (it != bindings.end())
				bindings.erase(it);

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
			// TODO

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
		// </Example>


	public:
		void setDataContext(std::shared_ptr<IPropertyHost> _dataContext)
		{
			dataContext = _dataContext;
			onDataContextChanged();
		}
	protected:
		virtual void onDataContextChanged()
		{
			for (auto&& bindingLink : bindings)
			{
				se_assert(bindingLink);
				resolveBinding(*bindingLink);
			}
		}

	protected:
		virtual IPropertyHost* findBindingSource(const Binding& _binding)
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

			if (parent)
			{
				return parent->findBindingSource(_binding);
			}
			return nullptr;
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

	protected:
		bool routeParent(const EventRoutingFunction& _func, const EventArgs& _args) override
		{
			if (parent)
			{
				if (_func(static_cast<IEventRouter*>(parent), _args))
					return true;
			}
			return false;
		}

	protected:
		std::string name;
		Element* parent = nullptr;
		std::shared_ptr<IPropertyHost> dataContext;
		std::vector<std::unique_ptr<BindingLink>> bindings;
	};
}
