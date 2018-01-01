#pragma once
#include <cmath>
#include <string>
#include <type_traits>
/*
	TODO:
		isNull()
		dot(vec& other)
		cross(vec& other) not currently used anywhere...
*/
namespace spehs
{
	template<typename T>
	class Vector4
	{
		static_assert(std::is_arithmetic<T>::value, "spehs::Vector4 type T must be arithmetic!");
	public:
		static const Vector4<T> zero;

	public:

		Vector4()
			: x((T)0), y((T)0), z((T)0), w((T)0)
		{//Default constructor
		}

		Vector4(const T _x, const T _y, const T _z, const T _w)
			: x(_x), y(_y), z(_z), w(_w)
		{//Member initializizer constructor
		}

		Vector4<T> operator-() const
		{
			//static_assert(std::is_signed<T>::value, "spehs::Vector4 unary - operator should only be used on Vectors with a signed component type");
			return Vector4(-x, -y, -z, -w);
		}

		template<typename ComponentType>
		operator Vector4<ComponentType>() const
		{
			//static_assert(!(std::is_floating_point<T>::value && std::is_integral<ComponentType>::value), "spehs::Vector4 casting error! Should not cast from floating point component type into integral! User must manually handle the rounding in per case!");
			return Vector4<ComponentType>((ComponentType)x, (ComponentType)y, (ComponentType)z, (ComponentType)w);
		}
		
		template<typename T2>
		bool operator==(const Vector4<T2>& other) const
		{
			return std::is_same<T, T2>::value
				&& x == other.x
				&& y == other.y
				&& z == other.z
				&& w == other.w;
		}

		template<typename T2>
		bool operator!=(const Vector4<T2>& other) const
		{
			return !std::is_same<T, T2>::value
				|| x != other.x
				|| y != other.y
				|| z != other.z
				|| w != other.w;
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

		Vector4<T> operator+(const Vector4<T>& other) const
		{
			return Vector4<T>(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		Vector4<T> operator-(const Vector4<T>& other) const
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
			static_assert(std::is_floating_point<T>::value, "spehs::Vector4 type T must be floating point to be able to evaluate length! Otherwise user must implement his/her own solution.");
			return std::pow(x * x + y * y + z * z + w * w, 0.5f);
		}

		T dot(const Vector4<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector4 type T must be floating point to be able to evaluate dot product! Otherwise user must implement his/her own solution.");
			return x * other.x + y * other.y + z * other.z + w * other.w;
		}

		bool isNull(const T epsilon)
		{
			return
				abs(x) <= epsilon &&
				abs(y) <= epsilon &&
				abs(z) <= epsilon &&
				abs(w) <= epsilon;
		}

		Vector4<T>& normalize()
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector4 type T must be floating point to be able to normalize! Otherwise user must implement his/her own solution.");
			const T totalLength = abs(x) + abs(y) + abs(z) + abs(w);
			if (totalLength == (T)0)
				return *this;
			x /= totalLength;
			y /= totalLength;
			z /= totalLength;
			w /= totalLength;
			return *this;
		}

		Vector4<T> getNormalized() const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector4 type T must be floating point to be able to get normalized! Otherwise user must implement his/her own solution.");
			const T totalLength = abs(x) + abs(y) + abs(z) + abs(w);
			if (totalLength == (T)0)
				return *this;
			return Vector4<T>(x / totalLength, y / totalLength, z / totalLength, w / totalLength);
		}

		T distance(const Vector4<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector4 type T must be floating point to be able to evaluate distance! Otherwise user must implement his/her own solution.");
			const T dx = x - other.x;
			const T dy = y - other.y;
			const T dz = z - other.z;
			const T dw = w - other.w;
			return std::pow(x * x + y * y + z * z + w * w, (T)0.5);
		}

		union { T x, r, u; };
		union { T y, g, v; };
		union { T z, b; };
		union { T w, a; };
	};
	template<typename T>
	Vector4<T> operator/(const float f, const Vector4<T>& vec)
	{
		return Vector4<T>(vec.x / f, vec.y / f, vec.z / f, vec.w / f);
	}
	template<typename T>
	Vector4<T> operator*(const float f, const Vector4<T>& vec)
	{
		return Vector4<T>(vec.x * f, vec.y * f, vec.z * f, vec.w * f);
	}

	template<typename T>
	class Vector3
	{
		static_assert(std::is_arithmetic<T>::value, "spehs::Vector3 type T must be arithmetic!");
	public:
		static const Vector3<T> zero;

	public:

		Vector3()
			: x((T)0), y((T)0), z((T)0)
		{//Default constructor
		}

		Vector3(const T _x, const T _y, const T _z)
			: x(_x), y(_y), z(_z)
		{//Member initializizer constructor
		}

		Vector3<T> operator-() const
		{
			//static_assert(std::is_signed<T>::value, "spehs::Vector3 unary - operator should only be used on Vectors with a signed component type");
			return Vector3(-x, -y, -z);
		}

		template<typename ComponentType>
		operator Vector3<ComponentType>() const
		{
			//static_assert(!(std::is_floating_point<T>::value && std::is_integral<ComponentType>::value), "spehs::Vector3 casting error! Should not cast from floating point component type into integral! User must manually handle the rounding in per case!");
			return Vector3<ComponentType>((ComponentType)x, (ComponentType)y, (ComponentType)z);
		}

		template<typename T2>
		bool operator==(const Vector3<T2>& other) const
		{
			return std::is_same<T, T2>::value
				&& x == other.x
				&& y == other.y
				&& z == other.z;
		}

		template<typename T2>
		bool operator!=(const Vector3<T2>& other) const
		{
			return !std::is_same<T, T2>::value
				|| x != other.x
				|| y != other.y
				|| z != other.z;
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

		Vector3<T> operator+(const Vector3<T>& other) const
		{
			return Vector3<T>(x + other.x, y + other.y, z + other.z);
		}

		Vector3<T> operator-(const Vector3<T>& other) const
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
			static_assert(std::is_floating_point<T>::value, "spehs::Vector3 type T must be floating point to be able to evaluate length! Otherwise user must implement his/her own solution.");
			return std::pow(x * x + y * y + z * z, 0.5f);
		}

		T dot(const Vector3<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector3 type T must be floating point to be able to evaluate dot product! Otherwise user must implement his/her own solution.");
			return x * other.x + y * other.y + z * other.z;
		}

		bool isNull(const T epsilon)
		{
			return
				abs(x) <= epsilon &&
				abs(y) <= epsilon &&
				abs(z) <= epsilon;
		}

		Vector3<T>& normalize()
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector3 type T must be floating point to be able to normalize! Otherwise user must implement his/her own solution.");
			const T totalLength = abs(x) + abs(y) + abs(z);
			if (totalLength == (T)0)
				return *this;
			x /= totalLength;
			y /= totalLength;
			z /= totalLength;
			return *this;
		}

		Vector3<T> getNormalized() const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector3 type T must be floating point to be able to get normalized! Otherwise user must implement his/her own solution.");
			const T totalLength = abs(x) + abs(y) + abs(z);
			if (totalLength == (T)0)
				return *this;
			return Vector3<T>(x / totalLength, y / totalLength, z / totalLength);
		}

		T distance(const Vector3<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector3 type T must be floating point to be able to evaluate distance! Otherwise user must implement his/her own solution.");
			const T dx = x - other.x;
			const T dy = y - other.y;
			const T dz = z - other.z;
			return std::pow(x * x + y * y + z * z, (T)0.5);
		}
		
		Vector3<T> cross(const Vector3<T>& other) const
		{
		//	static_assert(std::is_floating_point<T>::value, "spehs::Vector3 type T must be floating point to be able to evaluate cross product! Otherwise user must implement his/her own solution.");
		//	const T myLength = getLength();
		//	const T otherLength = other.getLength();
		//	// return myLength * otherLength * sin(getAngle(other)) * n(unit vector perpendicular to plane formed by this and other)
			return Vector3<T>(
				y * other.z - other.y * z,
				z * other.x - other.z * x,
				x * other.y - other.x * y);
		}

		T getAngle(const Vector3<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector3 type T must be floating point to be able to evaluate angle between! Otherwise user must implement his/her own solution.");
			std::acos(dot(other) / (getLength() * other.getLength()));
		}

		union { T x, r, u; };
		union { T y, g, v; };
		union { T z, b; };
	};
	template<typename T>
	Vector3<T> operator/(const float f, const Vector3<T>& vec)
	{
		return Vector3<T>(vec.x / f, vec.y / f, vec.z / f);
	}
	template<typename T>
	Vector3<T> operator*(const float f, const Vector3<T>& vec)
	{
		return Vector3<T>(vec.x * f, vec.y * f, vec.z * f);
	}


	template<typename T>
	class Vector2
	{
		static_assert(std::is_arithmetic<T>::value, "spehs::Vector2 type T must be arithmetic!");
	public:
		static const Vector2<T> zero;

	public:

		Vector2()
			: x((T)0), y((T)0)
		{//Default constructor
		}

		Vector2(const T _x, const T _y)
			: x(_x), y(_y)
		{//Member initializizer constructor
		}

		Vector2<T> operator-() const
		{
			//static_assert(std::is_signed<T>::value, "spehs::Vector2 unary - operator should only be used on Vectors with a signed component type");
			return Vector2(-x, -y);
		}

		template<typename ComponentType>
		operator Vector2<ComponentType>() const
		{
			//static_assert(std::is_floating_point<T>::value && std::is_integral<ComponentType>::value, "spehs::Vector2 casting error! Should not cast from floating point component type into integral! User must manually handle the rounding in per case!");
			return Vector2<ComponentType>((ComponentType)x, (ComponentType)y);
		}

		template<typename T2>
		bool operator==(const Vector2<T2>& other) const
		{
			return std::is_same<T, T2>::value
				&& x == other.x
				&& y == other.y;
		}

		template<typename T2>
		bool operator!=(const Vector2<T2>& other) const
		{
			return !std::is_same<T, T2>::value
				|| x != other.x
				|| y != other.y;
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

		Vector2<T> operator+(const Vector2<T>& other) const
		{
			return Vector2<T>(x + other.x, y + other.y);
		}

		Vector2<T> operator-(const Vector2<T>& other) const
		{
			return Vector2<T>(x - other.x, y - other.y);
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
			static_assert(std::is_floating_point<T>::value, "spehs::Vector2 type T must be floating point to be able to evaluate length! Otherwise user must implement his/her own solution.");
			return std::pow(x * x + y * y, 0.5f);
		}

		T dot(const Vector2<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector2 type T must be floating point to be able to evaluate dot product! Otherwise user must implement his/her own solution.");
			return x * other.x + y * other.y;
		}

		bool isNull(const T epsilon)
		{
			return
				abs(x) <= epsilon &&
				abs(y) <= epsilon;
		}

		Vector2<T>& normalize()
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector2 type T must be floating point to be able to normalize! Otherwise user must implement his/her own solution.");
			const T totalLength = abs(x) + abs(y);
			if (totalLength == (T)0)
				return *this;
			x /= totalLength;
			y /= totalLength;
			return *this;
		}

		Vector2<T> getNormalized() const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector2 type T must be floating point to be able to get normalized! Otherwise user must implement his/her own solution.");
			const T totalLength = abs(x) + abs(y);
			if (totalLength == (T)0)
				return *this;
			return Vector2<T>(x / totalLength, y / totalLength);
		}

		T distance(const Vector2<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector2 type T must be floating point to be able to evaluate distance! Otherwise user must implement his/her own solution.");
			const T dx = x - other.x;
			const T dy = y - other.y;
			return std::pow(x * x + y * y, (T)0.5);
		}

		T getAngle(const Vector2<T>& other) const
		{
			static_assert(std::is_floating_point<T>::value, "spehs::Vector2 type T must be floating point to be able to evaluate angle between! Otherwise user must implement his/her own solution.");
			std::acos(dot(other) / (getLength() * other.getLength()));
		}

		union { T x, u; };
		union { T y, v; };
	};
	template<typename T>
	Vector2<T> operator/(const float f, const Vector2<T>& vec)
	{
		return Vector2<T>(vec.x / f, vec.y / f);
	}
	template<typename T>
	Vector2<T> operator*(const float f, const Vector2<T>& vec)
	{
		return Vector2<T>(vec.x * f, vec.y * f);
	}

	typedef Vector4<float> vec4;
	typedef Vector4<int> ivec4;
	typedef Vector3<float> vec3;
	typedef Vector3<int> ivec3;
	typedef Vector2<float> vec2;
	typedef Vector2<int> ivec2;

	template<typename T>
	const Vector4<T> Vector4<T>::zero = Vector4(0, 0, 0, 0);
	template<typename T>
	const Vector3<T> Vector3<T>::zero = Vector3(0, 0, 0);
	template<typename T>
	const Vector2<T> Vector2<T>::zero = Vector2(0, 0);
}