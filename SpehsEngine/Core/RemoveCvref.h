#pragma once


namespace se
{
	// Combines std::remove_cv and std::remove_reference 
	template<typename T>
	struct remove_cvref
	{
		typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
	};
}
