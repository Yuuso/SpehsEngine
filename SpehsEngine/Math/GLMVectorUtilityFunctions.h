#pragma once


namespace se
{
	std::string toString(const glm::vec2& vec);
	std::string toString(const glm::vec3& vec);
	std::string toString(const glm::vec4& vec);
	std::string toString(const glm::ivec2& vec);
	std::string toString(const glm::ivec3& vec);
	std::string toString(const glm::ivec4& vec);

	// Serial
	struct SerialTagVec2 {};
	struct SerialTagVec3 {};
	struct SerialTagVec4 {};
	template<typename T, glm::qualifier Q> struct SerialTag<glm::vec<2, T, Q>> { typedef SerialTagVec2 type; };
	template<typename T, glm::qualifier Q> struct SerialTag<glm::vec<3, T, Q>> { typedef SerialTagVec3 type; };
	template<typename T, glm::qualifier Q> struct SerialTag<glm::vec<4, T, Q>> { typedef SerialTagVec4 type; };
	template<> template<typename S, typename T>
	static bool Serial<SerialTagVec2>::serial(S& _serial, T _value)
	{
		se_serial(_serial, _value.x, "x");
		se_serial(_serial, _value.y, "y");
		return true;
	}
	template<> template<typename S, typename T>
	static bool Serial<SerialTagVec3>::serial(S& _serial, T _value)
	{
		se_serial(_serial, _value.x, "x");
		se_serial(_serial, _value.y, "y");
		se_serial(_serial, _value.z, "z");
		return true;
	}
	template<> template<typename S, typename T>
	static bool Serial<SerialTagVec4>::serial(S& _serial, T _value)
	{
		se_serial(_serial, _value.x, "x");
		se_serial(_serial, _value.y, "y");
		se_serial(_serial, _value.z, "z");
		se_serial(_serial, _value.w, "w");
		return true;
	}
}

namespace std
{
	static_assert(sizeof(glm::ivec2) == sizeof(uint64_t));
	static_assert(sizeof(glm::vec2) == sizeof(uint64_t));

	template<> struct hash<glm::vec2>
	{
		size_t operator()(const glm::vec2& vec) const
		{
			const uint64_t value = (uint64_t&)vec;
			return std::hash<uint64_t>()(value);
		}
	};

	template<> struct hash<glm::ivec2>
	{
		size_t operator()(const glm::ivec2& vec) const
		{
			const uint64_t value = (uint64_t&)vec;
			return std::hash<uint64_t>()(value);
		}
	};

	template<> struct less<glm::vec2>
	{
		size_t operator()(const glm::vec2& a, const glm::vec2& b) const
		{
			const uint64_t value1 = (uint64_t&)a;
			const uint64_t value2 = (uint64_t&)b;
			return value1 < value2;
		}
	};

	template<> struct less<glm::ivec2>
	{
		size_t operator()(const glm::ivec2& a, const glm::ivec2& b) const
		{
			const uint64_t value1 = (uint64_t&)a;
			const uint64_t value2 = (uint64_t&)b;
			return value1 < value2;
		}
	};
}
