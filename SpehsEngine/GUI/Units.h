#pragma once

#include "glm/vec2.hpp"


namespace se
{
	namespace gui
	{
		enum class GUIUnitType
		{
			Undefined,

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

			GUIUnitType type = GUIUnitType::Pixels;
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
			inline GUIUnit operator "" _px(long double _value)			{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Pixels); }
			inline GUIUnit operator "" _px(unsigned long long _value)	{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Pixels); }
		}
	}
}