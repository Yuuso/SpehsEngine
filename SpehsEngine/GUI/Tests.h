#pragma once

#include "SpehsEngine/GUI/Element.h"
#include "SpehsEngine/GUI/Layout.h"


namespace se::gui
{
	struct AppData
	{
		int value = 4;
	};

	class ViewModel : public IPropertyHost
	{
		GUI_PROPERTY_CLASS(ViewModel)

	public:
		ViewModel(AppData& _data)
			: data(_data) {}
		AppData& data;

	private:
		static std::any property_DataValueGet(const IPropertyHost* _instance)
		{
			return static_cast<const SelfClass*>(_instance)->getDataValue();
		}
		static void property_DataValueSet(IPropertyHost* _instance, const std::any& _value)
		{
			try
			{
				static_cast<SelfClass*>(_instance)->setDataValue(std::any_cast<const int&>(_value));
			}
			catch (const std::bad_any_cast&)
			{
				se::log::warning("Invalid 'DataValue' property value type!");
			}
		}
		inline static const PropertyLink property_DataValueLink{property_Map, "DataValue", property_DataValueGet, property_DataValueSet, 0};
		bool property_DataValueSetterGuard = false;
	public:
		const int& getDataValue() const { return data.value; }
		SelfClass& setDataValue(const int& _value)
		{
			if (property_DataValueSetterGuard)
			{
				// To prevent recursion
				return *this;
			}
			if (_value == data.value)
			{
				return *this;
			}
			property_DataValueSetterGuard = true;

			data.value = _value;
			propertyChangedSignal("DataValue", _value);

			property_DataValueSetterGuard = false;
			return *this;
		}
	};


	class TestApp
	{
	public:
		TestApp(input::EventSignaler& _eventSignaler)
			: layout(_eventSignaler, 1)
		{}

		void init()
		{
			viewModel = std::make_shared<ViewModel>(data);

			Element& element = *layout.add<Element>();

			element.onMouseMotion([]{ log::info("boo"); return false; });
			element.onMouseLeftPress([](const MouseButtonArgs& args){ log::info("hoo " + toString(args.position)); return false; });

			se_assert(element.getExample() == 0);
			element.setExample(3);
			se_assert(element.getExample() == 3);

			element.setExample(Binding("DataValue", BindingMode::TwoWay));
			se_assert(element.getExample() == 0);

			element.setDataContext(viewModel);
			se_assert(element.getExample() == 4);

			viewModel->setDataValue(5);
			se_assert(element.getExample() == 5);

			element.setExample(6);
			se_assert(viewModel->getDataValue() == 6);

			element.setDataContext(nullptr);
			se_assert(element.getExample() == 0);

			element.setExample(Binding("DataValue", BindingMode::OneWayToSource));
			se_assert(element.getExample() == 0);

			element.setDataContext(viewModel);
			se_assert(element.getExample() == 0);

			element.setExample(6);
			se_assert(viewModel->getDataValue() == 6);

			viewModel->setDataValue(7);
			se_assert(element.getExample() == 6);

			element.setDataContext(nullptr);
			se_assert(element.getExample() == 6);
		}
		void run()
		{
		}

	private:
		AppData data;
		std::shared_ptr<ViewModel> viewModel;
		Layout layout;
	};
}
