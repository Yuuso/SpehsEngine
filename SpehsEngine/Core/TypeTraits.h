#pragma once


namespace se
{
	template<typename>
	struct IsUniquePtr : std::false_type {};
	template<typename T, typename D>
	struct IsUniquePtr<std::unique_ptr<T, D>> : std::true_type {};
}
