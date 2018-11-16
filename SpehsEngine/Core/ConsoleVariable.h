#pragma once
#include <string>
#include <vector>
#include <functional>

namespace se
{
	template <typename T>
	struct ConsoleVariable
	{
		ConsoleVariable(std::string identifier, T& ptr)
		{
			_identifier = identifier;
			_variablePtr = &ptr;
		}
		void set(T value)
		{
			*_variablePtr = value;
		}
		std::string _identifier;
		T* _variablePtr = nullptr;
	};

	struct ConsoleCommand
	{
		ConsoleCommand(std::string identifier, const std::function<void()>& callback)
			: callback1(callback)
			, identifier(identifier)
		{
		}

		ConsoleCommand(std::string identifier, const std::function<void(std::vector<std::string>&)>& callback)
			: callback2(callback)
			 , identifier(identifier)
		{
		}

		std::string identifier;
		std::function<void()> callback1;
		std::function<void(std::vector<std::string>&)> callback2;
	};
}