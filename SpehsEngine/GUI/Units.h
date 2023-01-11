#pragma once

#include "glm/vec2.hpp"
#include <stdint.h>


namespace se
{
	namespace gui
	{
		enum class UnitType : int8_t
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

		struct Unit
		{
			constexpr Unit() = default;
			constexpr Unit(UnitType _type)
				: type(_type) {}
			constexpr Unit(float _value, UnitType _type)
				: value(_value), type(_type) {}

			bool operator==(const Unit& _other) const				{ return _other.value == this->value && _other.type == this->type; }
			bool operator!=(const Unit& _other) const				{ return _other.value != this->value || _other.type != this->type; }
			bool operator>(const Unit& _other) const				{ se_assert(this->type == _other.type); return this->value > _other.value; }
			bool operator>=(const Unit& _other) const				{ se_assert(this->type == _other.type); return this->value >= _other.value; }
			bool operator<(const Unit& _other) const				{ se_assert(this->type == _other.type); return this->value < _other.value; }
			bool operator<=(const Unit& _other) const				{ se_assert(this->type == _other.type); return this->value <= _other.value; }

			void operator+=(const Unit& _other)						{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); this->value += _other.value; }
			void operator-=(const Unit& _other)						{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); this->value -= _other.value; }
			void operator*=(const Unit& _other)						{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); this->value *= _other.value; }
			void operator/=(const Unit& _other)						{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); this->value /= _other.value; }
			void operator+=(float _value)							{ this->value += _value; }
			void operator-=(float _value)							{ this->value -= _value; }
			void operator*=(float _value)							{ this->value *= _value; }
			void operator/=(float _value)							{ this->value /= _value; }
			Unit operator+(const Unit& _other) const				{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); return Unit(this->value + _other.value, this->type); }
			Unit operator-(const Unit& _other) const				{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); return Unit(this->value - _other.value, this->type); }
			Unit operator*(const Unit& _other) const				{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); return Unit(this->value * _other.value, this->type); }
			Unit operator/(const Unit& _other) const				{ se_assert(this->type == _other.type || _other.type == UnitType::Undefined); return Unit(this->value / _other.value, this->type); }
			Unit operator+(float _value) const						{ return Unit(this->value + _value, this->type); }
			Unit operator-(float _value) const						{ return Unit(this->value - _value, this->type); }
			Unit operator*(float _value) const						{ return Unit(this->value * _value, this->type); }
			Unit operator/(float _value) const						{ return Unit(this->value / _value, this->type); }

			UnitType type = UnitType::Undefined;
			float value = 0.0f;
		};

		struct Vec2
		{
			constexpr Vec2() = default;
			constexpr Vec2(const glm::vec2& _value, UnitType _type)
				: x(_value.x, _type), y(_value.y, _type) {}
			constexpr Vec2(Unit _value)
				: x(_value), y(_value) {}
			constexpr Vec2(Unit _x, Unit _y)
				: x(_x), y(_y) {}
			constexpr Vec2(UnitType _type)
				: x(0.0f, _type), y(0.0f, _type) {}

			bool operator==(const Vec2& _other) const				{ return _other.x == this->x && _other.y == this->y; }
			bool operator!=(const Vec2& _other) const				{ return _other.x != this->x || _other.y != this->y; }

			void operator+=(const Vec2& _other)						{ this->x += _other.x; this->y += _other.y; }
			void operator-=(const Vec2& _other)						{ this->x -= _other.x; this->y -= _other.y; }
			void operator*=(const Vec2& _other)						{ this->x *= _other.x; this->y *= _other.y; }
			void operator/=(const Vec2& _other)						{ this->x /= _other.x; this->y /= _other.y; }
			void operator*=(float _value)							{ this->x *= _value; this->y *= _value; }
			void operator/=(float _value)							{ this->x /= _value; this->y /= _value; }
			Vec2 operator+(const Vec2& _other) const				{ return Vec2(this->x + _other.x, this->y + _other.y); }
			Vec2 operator-(const Vec2& _other) const				{ return Vec2(this->x - _other.x, this->y - _other.y); }
			Vec2 operator*(const Vec2& _other) const				{ return Vec2(this->x * _other.x, this->y * _other.y); }
			Vec2 operator/(const Vec2& _other) const				{ return Vec2(this->x / _other.x, this->y / _other.y); }
			Vec2 operator*(float _value) const						{ return Vec2(this->x * _value, this->y * _value); }
			Vec2 operator/(float _value) const						{ return Vec2(this->x / _value, this->y / _value); }

			Unit x;
			Unit y;
		};


		namespace unit_literals
		{
			inline constexpr Unit operator "" _px(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::Pixels); }
			inline constexpr Unit operator "" _px(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::Pixels); }

			inline constexpr Unit operator "" _self(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::Self); }
			inline constexpr Unit operator "" _self(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::Self); }
			inline constexpr Unit operator "" _sw(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::SelfWidth); }
			inline constexpr Unit operator "" _sw(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::SelfWidth); }
			inline constexpr Unit operator "" _sh(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::SelfHeight); }
			inline constexpr Unit operator "" _sh(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::SelfHeight); }
			inline constexpr Unit operator "" _smin(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::SelfMin); }
			inline constexpr Unit operator "" _smin(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::SelfMin); }
			inline constexpr Unit operator "" _smax(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::SelfMax); }
			inline constexpr Unit operator "" _smax(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::SelfMax); }

			inline constexpr Unit operator "" _parent(long double _value)						{ return Unit(static_cast<float>(_value), UnitType::Parent); }
			inline constexpr Unit operator "" _parent(unsigned long long _value)				{ return Unit(static_cast<float>(_value), UnitType::Parent); }
			inline constexpr Unit operator "" _pw(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ParentWidth); }
			inline constexpr Unit operator "" _pw(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ParentWidth); }
			inline constexpr Unit operator "" _ph(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ParentHeight); }
			inline constexpr Unit operator "" _ph(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ParentHeight); }
			inline constexpr Unit operator "" _pmin(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ParentMin); }
			inline constexpr Unit operator "" _pmin(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ParentMin); }
			inline constexpr Unit operator "" _pmax(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ParentMax); }
			inline constexpr Unit operator "" _pmax(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ParentMax); }

			inline constexpr Unit operator "" _view(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::View); }
			inline constexpr Unit operator "" _view(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::View); }
			inline constexpr Unit operator "" _vw(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ViewWidth); }
			inline constexpr Unit operator "" _vw(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ViewWidth); }
			inline constexpr Unit operator "" _vh(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ViewHeight); }
			inline constexpr Unit operator "" _vh(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ViewHeight); }
			inline constexpr Unit operator "" _vmin(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ViewMin); }
			inline constexpr Unit operator "" _vmin(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ViewMin); }
			inline constexpr Unit operator "" _vmax(long double _value)							{ return Unit(static_cast<float>(_value), UnitType::ViewMax); }
			inline constexpr Unit operator "" _vmax(unsigned long long _value)					{ return Unit(static_cast<float>(_value), UnitType::ViewMax); }
		}
	}

	namespace legacygui
	{
		typedef gui::UnitType	GUIUnitType;
		typedef gui::Unit		GUIUnit;
		typedef gui::Vec2		GUIVec2;
	}
}