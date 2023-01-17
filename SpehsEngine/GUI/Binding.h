#pragma once

#include "stdafx.h" //
#include <any>


namespace se::gui
{
	class PropertyLink;

	enum class BindingMode
	{
		Default,			// Element decides, usually OneWay
		OneTime,
		OneWay,
		TwoWay,
		OneWayToSource,
	};

	enum class BindingSourceUpdate
	{
		Default,			// Element decides, usually PropertyChanged
		PropertyChanged,
	};

	class Binding
	{
	public:
		Binding(std::string_view _propertyName, BindingMode _mode = BindingMode::Default, BindingSourceUpdate _sourceUpdate = BindingSourceUpdate::Default)
			: sourcePropertyName(_propertyName), mode(_mode), sourceUpdate(_sourceUpdate) {}

		std::string sourcePropertyName;
		std::string sourceElementName;
		BindingMode mode;
		BindingSourceUpdate sourceUpdate;
		//IConverter converter;
	};

	class BindingLink
	{
	public:
		BindingLink(const Binding& _binding, const PropertyLink& _link)
			: binding(_binding), propertyLink(_link) {}

		const Binding binding;
		const PropertyLink& propertyLink;
		ScopedConnection sourceChangedConnection;
		ScopedConnection targetChangedConnection;
	};

	static inline bool bindingShouldUpdateSourceOnPropertyChanged(const Binding& _binding)
	{
		if (_binding.sourceUpdate == BindingSourceUpdate::PropertyChanged)
		{
			return _binding.mode == BindingMode::TwoWay ||
				_binding.mode == BindingMode::OneWayToSource;
		}
		return false;
	}
	static inline bool bindingShouldUpdateTargetInitial(const Binding& _binding)
	{
		return _binding.mode != BindingMode::OneWayToSource;
	}
	static inline bool bindingShouldUpdateTargetOnPropertyChanged(const Binding& _binding)
	{
		return _binding.mode == BindingMode::OneWay || _binding.mode == BindingMode::TwoWay;
	}
	static inline Binding bindingResolveDefaultValue(
		const Binding& _binding, BindingMode mode, BindingSourceUpdate sourceUpdate)
	{
		Binding result = _binding;
		if (result.mode == BindingMode::Default)
		{
			result.mode = mode;
		}
		if (result.sourceUpdate == BindingSourceUpdate::Default)
		{
			result.sourceUpdate = sourceUpdate;
		}
		return result;
	}
}
