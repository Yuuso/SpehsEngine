#pragma once

#include "glm/vec2.hpp"
#include <stdint.h>


namespace se
{
	namespace gui
	{
		enum class GUIUnitType : int8_t
		{
			Undefined = -1,

			// Absolute
			Pixels,

			// Relative
			Self,
			SelfWidth,
			SelfHeight,
			SelfMin,
			SelfMax,

			Parent,
			ParentWidth,
			ParentHeight,
			ParentMin,
			ParentMax,

			View,
			ViewWidth,
			ViewHeight,
			ViewMin,
			ViewMax,
		};

		class GUIUnit
		{
		public:

			GUIUnit() = default;
			GUIUnit(float _value)
				: value(_value) {}
			GUIUnit(float _value, GUIUnitType _type)
				: value(_value), type(_type) {}

			operator float() const { return value; }

			GUIUnitType type = GUIUnitType::Undefined;
			float value = 0.0f;
		};

		class GUIVec2
		{
		public:

			GUIVec2() = default;
			GUIVec2(const glm::vec2& _value)
				: x(_value.x), y(_value.y) {}
			GUIVec2(const glm::vec2& _value, GUIUnitType _type)
				: x(_value.x, _type), y(_value.y, _type) {}
			GUIVec2(GUIUnit _x, GUIUnit _y)
				: x(_x), y(_y) {}

			inline operator glm::vec2() const { return glm::vec2(x, y); }
			inline glm::vec2 value() const { return glm::vec2(x, y); }

			GUIUnit x;
			GUIUnit y;
		};


		namespace unit_literals
		{
			inline GUIUnit operator "" _px(long double _value)								{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Pixels); }
			inline GUIUnit operator "" _px(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Pixels); }

			inline GUIUnit operator "" _self(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Self); }
			inline GUIUnit operator "" _self(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Self); }
			inline GUIUnit operator "" _sw(long double _value)								{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfWidth); }
			inline GUIUnit operator "" _sw(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfWidth); }
			inline GUIUnit operator "" _sh(long double _value)								{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfHeight); }
			inline GUIUnit operator "" _sh(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfHeight); }
			inline GUIUnit operator "" _smin(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMin); }
			inline GUIUnit operator "" _smin(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMin); }
			inline GUIUnit operator "" _smax(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMax); }
			inline GUIUnit operator "" _smax(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMax); }

			inline GUIUnit operator "" _parent(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Parent); }
			inline GUIUnit operator "" _parent(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Parent); }
			inline GUIUnit operator "" _pw(long double _value)								{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentWidth); }
			inline GUIUnit operator "" _pw(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentWidth); }
			inline GUIUnit operator "" _ph(long double _value)								{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentHeight); }
			inline GUIUnit operator "" _ph(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentHeight); }
			inline GUIUnit operator "" _pmin(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMin); }
			inline GUIUnit operator "" _pmin(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMin); }
			inline GUIUnit operator "" _pmax(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMax); }
			inline GUIUnit operator "" _pmax(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMax); }

			inline GUIUnit operator "" _view(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::View); }
			inline GUIUnit operator "" _view(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::View); }
			inline GUIUnit operator "" _vw(long double _value)								{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewWidth); }
			inline GUIUnit operator "" _vw(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewWidth); }
			inline GUIUnit operator "" _vh(long double _value)								{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewHeight); }
			inline GUIUnit operator "" _vh(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewHeight); }
			inline GUIUnit operator "" _vmin(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMin); }
			inline GUIUnit operator "" _vmin(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMin); }
			inline GUIUnit operator "" _vmax(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMax); }
			inline GUIUnit operator "" _vmax(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMax); }
		}
	}
}