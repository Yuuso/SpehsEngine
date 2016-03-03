#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include "ConsoleVariable.h"
#define CONSOLE_FONT_COLOR 1.0f, 0.6f, 0.0f


namespace spehs
{
	class Text;
	class Console
	{
	public:
		Console();
		~Console();

		int initialize();
		void unitialize();

		void update();
		void render();
		void addVariable(std::string identifier, bool& var);
		void addVariable(std::string identifier, float& var);
		void addVariable(std::string identifier, int& var);
		void addVariable(std::string identifier, std::string& var);
		void addConsoleCommand(std::string identifier, void(*fnc)(std::vector<std::string>&));
		void log(std::string str, glm::vec3& color = glm::vec3(CONSOLE_FONT_COLOR));
		void warning(std::string warning);
		void error(std::string error);
		void fatalError(std::string fatal_error);
		void clearLog();
		void clearVariables();
		void clearCommands();
		void openConsole();
		void closeConsole();
		bool isOpen(){ return open; }

	private:
		float visibility = 1.0f;
		bool open = false;
		std::string input;
		Text* consoleText;
		std::vector<Text*> lines;
		int backspaceTimer = 0;
		int backspaceAcceleration = 0;
		std::vector<ConsoleCommand> commands;
		std::vector<std::string> consoleWords;
		std::vector<ConsoleVariable<int>> intVariables;
		std::vector<ConsoleVariable<float>> floatVariables;
		std::vector<ConsoleVariable<bool>> boolVariables;
		std::vector<ConsoleVariable<std::string>> stringVariables;
		std::vector<std::string> previousCommands;
		int previousCommandIndex = 0;

		void updateLinePositions();
		void setVariable();
		void executeConsole();
	};
}
extern spehs::Console* console;