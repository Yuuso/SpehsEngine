#pragma once
#include "Text.h"
#include <vector>
#include <string>


namespace SpehsEngine
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


	class Console
	{
	public:
		Console();
		~Console();

		int initialize();
		void unitialize();

		void update();
		void render();
		void addVariable(std::string str, bool& var);
		void addVariable(std::string str, float& var);
		void addVariable(std::string str, int& var);
		void addConsoleCommand(std::string str, void(*fnc)(std::vector<std::string>&));
		void log(std::string str);
		void clearLog();
		void openConsole();
		void closeConsole();

	private:
		float visibility = 1.0f;
		bool open = false;
		std::string input;
		Text* consoleText;
		std::vector<Text*> lines;
		float backspaceTimer = 0;
		int backspaceAcceleration = 0;
		std::vector<ConsoleCommand> commands;
		std::vector<std::string> consoleWords;
		std::vector<ConsoleVariable<int>> intVariables;
		std::vector<ConsoleVariable<float>> floatVariables;
		std::vector<ConsoleVariable<bool>> boolVariables;

		void updateLinePositions();
		void setVariable();
		void executeConsole();
	};
}
extern SpehsEngine::Console* console;