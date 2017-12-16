#pragma once
#include <string>
#include <vector>

namespace spehs
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
		ConsoleCommand(std::string identifier, void(*function)(void)) :
			_functionWords(nullptr),
			_functionVoid(function),
			_identifier(identifier)
		{
		}
		ConsoleCommand(std::string identifier, void(*function)(std::vector<std::string>& words)) :
			_functionWords(function),
			_functionVoid(nullptr),
			_identifier(identifier)			
		{
		}
		std::string _identifier;
		void(*_functionWords)(std::vector<std::string>& words);
		void(*_functionVoid)();
	};
}