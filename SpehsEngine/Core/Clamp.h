#pragma once


namespace se
{
	template<typename T, typename Compare>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare compare)
	{
		return compare(v, lo) ? lo : compare(hi, v) ? hi : v;
	}
	template<typename T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi)
	{
		return se::clamp<T, std::less<T>>(v, lo, hi, std::less<T>{});
	}
}
