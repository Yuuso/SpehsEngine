#pragma once
#include <string>
#include <type_traits>

namespace spehs
{
	template<typename T>
	class Vector4
	{
		static_assert(std::is_arithmetic<T>::value, "spehs::Vector type T must be arithmetic!");
	public:

		Vector4()
			: x((T)0), y((T)0), z((T)0), w((T)0)
		{//Default constructor
		}

		Vector4(const T _x, const T _y, const T _z, const T _w)
			: x(_x), y(_y), z(_z), w(_w)
		{//Member initializizer constructor
		}
		
		void operator+=(const Vector4<T>& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
		}

		void operator-=(const Vector4<T>& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
		}

		void operator*=(const T componentMultiplier)
		{
			x *= componentMultiplier;
			y *= componentMultiplier;
			z *= componentMultiplier;
			w *= componentMultiplier;
		}

		void operator/=(const T componentDivider)
		{
			x /= componentDivider;
			y /= componentDivider;
			z /= componentDivider;
			w /= componentDivider;
		}

		Vector4<T> operator+(const Vector4<T>& other)
		{
			return Vector4<T>(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		Vector4<T> operator-(const Vector4<T>& other)
		{
			return Vector4<T>(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		Vector4<T> operator*(const T componentMultiplier) const
		{
			return Vector4<T>(x * componentMultiplier, y * componentMultiplier, z * componentMultiplier, w * componentMultiplier);
		}

		Vector4<T> operator/(const T componentDivider) const
		{
			return Vector4<T>(x / componentDivider, y / componentDivider, z / componentDivider, w / componentDivider);
		}

		T& operator[](const int index)
		{
			return *((T*)this + index);
		}

		const T& operator[](const int index) const
		{
			return *((T*)this + index);
		}

		std::string toString() const
		{
			return "["
				+ std::to_string(this->operator[](0)) + ", "
				+ std::to_string(this->operator[](1)) + ", "
				+ std::to_string(this->operator[](2)) + ", "
				+ std::to_string(this->operator[](3)) + "]";
		}

		T getLengthSquared() const
		{
			return x * x + y * y + z * z + w * w;
		}

		T getLength() const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector type T must be floating point to be able to evaluate length! Otherwise user must implement his/her own solution.");
			return std::pow(x * x + y * y + z * z + w * w, 0.5f);
		}

		union { T x, r, u; };
		union { T y, g, v; };
		union { T z, b; };
		union { T w, a; };
	};

	template<typename T>
	class Vector3
	{
		static_assert(std::is_arithmetic<T>::value, "spehs::Vector type T must be arithmetic!");
	public:

		Vector3()
			: x((T)0), y((T)0), z((T)0)
		{//Default constructor
		}

		Vector3(const T _x, const T _y, const T _z)
			: x(_x), y(_y), z(_z)
		{//Member initializizer constructor
		}

		void operator+=(const Vector3<T>& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		void operator-=(const Vector3<T>& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		void operator*=(const T componentMultiplier)
		{
			x *= componentMultiplier;
			y *= componentMultiplier;
			z *= componentMultiplier;
		}

		void operator/=(const T componentDivider)
		{
			x /= componentDivider;
			y /= componentDivider;
			z /= componentDivider;
		}

		Vector3<T> operator+(const Vector3<T>& other)
		{
			return Vector3<T>(x + other.x, y + other.y, z + other.z);
		}

		Vector3<T> operator-(const Vector3<T>& other)
		{
			return Vector3<T>(x - other.x, y - other.y, z - other.z);
		}

		Vector3<T> operator*(const T componentMultiplier) const
		{
			return Vector3<T>(x * componentMultiplier, y * componentMultiplier, z * componentMultiplier);
		}

		Vector3<T> operator/(const T componentDivider) const
		{
			return Vector3<T>(x / componentDivider, y / componentDivider, z / componentDivider);
		}

		T& operator[](const int index)
		{
			return *((T*)this + index);
		}

		const T& operator[](const int index) const
		{
			return *((T*)this + index);
		}

		std::string toString() const
		{
			return "["
				+ std::to_string(this->operator[](0)) + ", "
				+ std::to_string(this->operator[](1)) + ", "
				+ std::to_string(this->operator[](2)) + "]";
		}

		T getLengthSquared() const
		{
			return x * x + y * y + z * z;
		}

		T getLength() const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector type T must be floating point to be able to evaluate length! Otherwise user must implement his/her own solution.");
			return std::pow(x * x + y * y + z * z, 0.5f);
		}

		union { T x, r, u; };
		union { T y, g, v; };
		union { T z, b; };
	};

	template<typename T>
	class Vector2
	{
		static_assert(std::is_arithmetic<T>::value, "spehs::Vector type T must be arithmetic!");
	public:

		Vector2()
			: x((T)0), y((T)0)
		{//Default constructor
		}

		Vector2(const T _x, const T _y)
			: x(_x), y(_y)
		{//Member initializizer constructor
		}

		void operator+=(const Vector2<T>& other)
		{
			x += other.x;
			y += other.y;
		}

		void operator-=(const Vector2<T>& other)
		{
			x -= other.x;
			y -= other.y;
		}

		void operator*=(const T componentMultiplier)
		{
			x *= componentMultiplier;
			y *= componentMultiplier;
		}

		void operator/=(const T componentDivider)
		{
			x /= componentDivider;
			y /= componentDivider;
		}

		Vector2<T> operator+(const Vector2<T>& other)
		{
			return Vector2<T>(x + other.x, y + other.y, z + other.z);
		}

		Vector2<T> operator-(const Vector2<T>& other)
		{
			return Vector2<T>(x - other.x, y - other.y, z - other.z);
		}

		Vector2<T> operator*(const T componentMultiplier) const
		{
			return Vector2<T>(x * componentMultiplier, y * componentMultiplier);
		}

		Vector2<T> operator/(const T componentDivider) const
		{
			return Vector2<T>(x / componentDivider, y / componentDivider);
		}

		T& operator[](const int index)
		{
			return *((T*)this + index);
		}

		const T& operator[](const int index) const
		{
			return *((T*)this + index);
		}

		std::string toString() const
		{
			return "["
				+ std::to_string(this->operator[](0)) + ", "
				+ std::to_string(this->operator[](1)) + "]";
		}

		T getLengthSquared() const
		{
			return x * x + y * y;
		}

		T getLength() const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector type T must be floating point to be able to evaluate length! Otherwise user must implement his/her own solution.");
			return std::pow(x * x + y * y, 0.5f);
		}

		union { T x, u; };
		union { T y, v; };
	};

	typedef Vector4<float> vec4;
	typedef Vector4<int> ivec4;
	typedef Vector3<float> vec3;
	typedef Vector3<int> ivec3;
	typedef Vector2<float> vec2;
	typedef Vector2<int> ivec2;
}