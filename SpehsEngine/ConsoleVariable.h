#pragma once
#include <string>
#include <vector>

namespace spehs
{
	template <typename T>
	struct ConsoleVariable
	{
		ConsoleVariable(std::string n, T& ptr)
		{
			identifier = n;
			variablePtr = &ptr;
		}
		void set(T value)
		{
			*variablePtr = value;
		}
		std::string identifier;
		T* variablePtr = nullptr;
	};
	struct ConsoleCommand
	{
		ConsoleCommand(std::string cmd, void(*fnc)(std::vector<std::string>& words))
		{
			command = cmd;
			function = fnc;
		}
		std::string command;
		void(*function)(std::vector<std::string>& words);
	};
}