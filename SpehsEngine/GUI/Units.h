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

			Auto,

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

		struct GUIUnit
		{
			constexpr GUIUnit() = default;
			constexpr GUIUnit(GUIUnitType _type)
				: type(_type) {}
			constexpr GUIUnit(float _value, GUIUnitType _type)
				: value(_value), type(_type) {}

			bool operator==(const GUIUnit& _other) const				{ return _other.value == this->value && _other.type == this->type; }
			bool operator!=(const GUIUnit& _other) const				{ return _other.value != this->value || _other.type != this->type; }
			bool operator>(const GUIUnit& _other) const					{ se_assert(this->type == _other.type); return this->value > _other.value; }
			bool operator>=(const GUIUnit& _other) const				{ se_assert(this->type == _other.type); return this->value >= _other.value; }
			bool operator<(const GUIUnit& _other) const					{ se_assert(this->type == _other.type); return this->value < _other.value; }
			bool operator<=(const GUIUnit& _other) const				{ se_assert(this->type == _other.type); return this->value <= _other.value; }

			void operator+=(const GUIUnit& _other)						{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); this->value += _other.value; }
			void operator-=(const GUIUnit& _other)						{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); this->value -= _other.value; }
			void operator*=(const GUIUnit& _other)						{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); this->value *= _other.value; }
			void operator/=(const GUIUnit& _other)						{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); this->value /= _other.value; }
			void operator+=(float _value)								{ this->value += _value; }
			void operator-=(float _value)								{ this->value -= _value; }
			void operator*=(float _value)								{ this->value *= _value; }
			void operator/=(float _value)								{ this->value /= _value; }
			GUIUnit operator+(const GUIUnit& _other) const				{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); return GUIUnit(this->value + _other.value, this->type); }
			GUIUnit operator-(const GUIUnit& _other) const				{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); return GUIUnit(this->value - _other.value, this->type); }
			GUIUnit operator*(const GUIUnit& _other) const				{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); return GUIUnit(this->value * _other.value, this->type); }
			GUIUnit operator/(const GUIUnit& _other) const				{ se_assert(this->type == _other.type || _other.type == GUIUnitType::Undefined); return GUIUnit(this->value / _other.value, this->type); }
			GUIUnit operator+(float _value) const						{ return GUIUnit(this->value + _value, this->type); }
			GUIUnit operator-(float _value) const						{ return GUIUnit(this->value - _value, this->type); }
			GUIUnit operator*(float _value) const						{ return GUIUnit(this->value * _value, this->type); }
			GUIUnit operator/(float _value) const						{ return GUIUnit(this->value / _value, this->type); }

			GUIUnitType type = GUIUnitType::Undefined;
			float value = 0.0f;
		};

		struct GUIVec2
		{
			constexpr GUIVec2() = default;
			constexpr GUIVec2(const glm::vec2& _value, GUIUnitType _type)
				: x(_value.x, _type), y(_value.y, _type) {}
			constexpr GUIVec2(GUIUnit _value)
				: x(_value), y(_value) {}
			constexpr GUIVec2(GUIUnit _x, GUIUnit _y)
				: x(_x), y(_y) {}
			constexpr GUIVec2(GUIUnitType _type)
				: x(0.0f, _type), y(0.0f, _type) {}

			bool operator==(const GUIVec2& _other) const				{ return _other.x == this->x && _other.y == this->y; }
			bool operator!=(const GUIVec2& _other) const				{ return _other.x != this->x || _other.y != this->y; }

			void operator+=(const GUIVec2& _other)						{ this->x += _other.x; this->y += _other.y; }
			void operator-=(const GUIVec2& _other)						{ this->x -= _other.x; this->y -= _other.y; }
			void operator*=(const GUIVec2& _other)						{ this->x *= _other.x; this->y *= _other.y; }
			void operator/=(const GUIVec2& _other)						{ this->x /= _other.x; this->y /= _other.y; }
			void operator*=(float _value)								{ this->x *= _value; this->y *= _value; }
			void operator/=(float _value)								{ this->x /= _value; this->y /= _value; }
			GUIVec2 operator+(const GUIVec2& _other) const				{ return GUIVec2(this->x + _other.x, this->y + _other.y); }
			GUIVec2 operator-(const GUIVec2& _other) const				{ return GUIVec2(this->x - _other.x, this->y - _other.y); }
			GUIVec2 operator*(const GUIVec2& _other) const				{ return GUIVec2(this->x * _other.x, this->y * _other.y); }
			GUIVec2 operator/(const GUIVec2& _other) const				{ return GUIVec2(this->x / _other.x, this->y / _other.y); }
			GUIVec2 operator*(float _value) const						{ return GUIVec2(this->x * _value, this->y * _value); }
			GUIVec2 operator/(float _value) const						{ return GUIVec2(this->x / _value, this->y / _value); }

			GUIUnit x;
			GUIUnit y;
		};


		namespace unit_literals
		{
			inline constexpr GUIUnit operator "" _px(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Pixels); }
			inline constexpr GUIUnit operator "" _px(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Pixels); }

			inline constexpr GUIUnit operator "" _self(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Self); }
			inline constexpr GUIUnit operator "" _self(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Self); }
			inline constexpr GUIUnit operator "" _sw(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfWidth); }
			inline constexpr GUIUnit operator "" _sw(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfWidth); }
			inline constexpr GUIUnit operator "" _sh(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfHeight); }
			inline constexpr GUIUnit operator "" _sh(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfHeight); }
			inline constexpr GUIUnit operator "" _smin(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMin); }
			inline constexpr GUIUnit operator "" _smin(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMin); }
			inline constexpr GUIUnit operator "" _smax(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMax); }
			inline constexpr GUIUnit operator "" _smax(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::SelfMax); }

			inline constexpr GUIUnit operator "" _parent(long double _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Parent); }
			inline constexpr GUIUnit operator "" _parent(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::Parent); }
			inline constexpr GUIUnit operator "" _pw(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentWidth); }
			inline constexpr GUIUnit operator "" _pw(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentWidth); }
			inline constexpr GUIUnit operator "" _ph(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentHeight); }
			inline constexpr GUIUnit operator "" _ph(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentHeight); }
			inline constexpr GUIUnit operator "" _pmin(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMin); }
			inline constexpr GUIUnit operator "" _pmin(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMin); }
			inline constexpr GUIUnit operator "" _pmax(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMax); }
			inline constexpr GUIUnit operator "" _pmax(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ParentMax); }

			inline constexpr GUIUnit operator "" _view(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::View); }
			inline constexpr GUIUnit operator "" _view(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::View); }
			inline constexpr GUIUnit operator "" _vw(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewWidth); }
			inline constexpr GUIUnit operator "" _vw(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewWidth); }
			inline constexpr GUIUnit operator "" _vh(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewHeight); }
			inline constexpr GUIUnit operator "" _vh(unsigned long long _value)						{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewHeight); }
			inline constexpr GUIUnit operator "" _vmin(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMin); }
			inline constexpr GUIUnit operator "" _vmin(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMin); }
			inline constexpr GUIUnit operator "" _vmax(long double _value)							{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMax); }
			inline constexpr GUIUnit operator "" _vmax(unsigned long long _value)					{ return GUIUnit(static_cast<float>(_value), GUIUnitType::ViewMax); }
		}
	}
}