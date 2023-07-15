#pragma once

#include "SpehsEngine/ImGui/Utility/Label.h"


namespace ImGui
{
	// Input tag types are used to allow making input functions for template classes, see stl container input functions as an example.
	template<typename ... Args>
	struct InputTag
	{
	};

	// The most commonly used default input tag is the type itself
	template<typename T>
	struct InputTag<T>
	{
		typedef T type;
	};

	// User's input implementations are specialized from this
	// T: type to be passed into InputTag<T> in order to choose the correct user implemenation
	template<typename T>
	struct Impl
	{
		template<typename V, typename ... Args>
		static bool input(const Label _label, V& _value, Args& ... _args);
	};

	// Call the input function
	template<typename T, typename ... Args>
	inline bool Input(const Label _label, T& _value, Args& ... _args)
	{
		return Impl<typename InputTag<T>::type>::input(_label, _value, std::forward<Args>(_args)...);
	}
}
