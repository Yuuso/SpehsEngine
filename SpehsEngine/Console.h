#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include "ConsoleVariable.h"
#define CONSOLE_FONT_COLOR 1.0f, 0.6f, 0.0f

/**Console is accessed through these functions.
The console is internally mutex locked and can thus be safely accessed from any thread.
*/
namespace spehs
{
	namespace console
	{
		//Console state
		int initialize();
		void unitialize();
		void open();
		void close();
		bool isOpen();

		//Update cycle
		void update();
		void render(std::string customDebugText = "");

		//Variables, commands
		void addVariable(std::string identifier, bool& var);
		void addVariable(std::string identifier, float& var);
		void addVariable(std::string identifier, int& var);
		void addVariable(std::string identifier, std::string& var);
		void addCommand(std::string identifier, void(*fnc)(void));
		void addCommand(std::string identifier, void(*fnc)(std::vector<std::string>&));
		bool removeCommand(std::string commandIdentifier);///< Returns true if command is found and removed succesfully
		bool removeVariable(std::string identifier);///< Returns true if variable with matching identifier was removed
		void clearVariables();
		void clearCommands();

		//Console log / exceptions
		void log(std::string str, glm::vec3& color = glm::vec3(CONSOLE_FONT_COLOR));
		void log(char* str, unsigned length, glm::vec3& color = glm::vec3(CONSOLE_FONT_COLOR));
		void warning(std::string warning);///<Exception: warning: inform through console. For direct warning without console, use spehs::exceptions::warning()
		void error(std::string error);///<Exception: error: inform through console. For direct error without console, use spehs::exceptions::unexpectedError()
		void clearLog();
		bool textEntered();///< Returns true if text was "executed" to console during the last update cycle (text without '/' (command) prefix)
		std::string getTextEntered();///< Returns latest text executed to console

		//Other
		void setPlaneDepth(int16_t depth);

		extern unsigned long drawCalls;
		extern unsigned long vertexDrawCount;
	}
}