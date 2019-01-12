#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <functional>
#include <stdint.h>
#include <boost/signal.hpp>
#include "SpehsEngine/Core/ConsoleVariable.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/SE_Time.h"
#define CONSOLE_FONT_COLOR 255, 153, 0

/**
	Console is accessed through these functions.
	The console is internally mutex locked and can thus be safely accessed from any thread.
*/
namespace se
{
	class Console
	{
	public:
		struct Line
		{
			std::string string;
			Color color;
		};
		static const Color defaultLogColor;
	public:

		Console();
		~Console();

		/* Logs string into console memory. String is not considered for execution. */
		void log(const std::string& string, const Color = defaultLogColor);
		/* Logs string into console memory. String is evaluated for execution. */
		void execute(const std::string& string, const Color = defaultLogColor);

		//Line access
		const Line& getLine(const size_t index) const;
		const size_t getLineCount() const;
		
		//Variables, commands
		void addVariable(const std::string& identifier, bool& var);
		void addVariable(const std::string& identifier, float& var);
		void addVariable(const std::string& identifier, int& var);
		void addVariable(const std::string& identifier, std::string& var);
		void addCommand(const std::string& identifier, const std::function<void(void)>& callback);
		void addCommand(const std::string& identifier, const std::function<void(const std::vector<std::string>&)>& callback);
		bool removeCommand(const std::string& commandIdentifier);///< Returns true if command is found and removed succesfully
		bool removeVariable(const std::string& identifier);///< Returns true if variable with matching identifier was removed

		//Clearing state
		void clearVariables();
		void clearCommands();
		void clearLog();
		
		boost::signal<void(const Line&) > logSignal;

	private:

		void setVariable();
		void executeConsole();

		mutable std::recursive_mutex mutex;
		std::vector<Line> lines;
		std::vector<ConsoleCommand> commands;
		std::vector<ConsoleVariable<int>> intVariables;
		std::vector<ConsoleVariable<float>> floatVariables;
		std::vector<ConsoleVariable<bool>> boolVariables;
		std::vector<ConsoleVariable<std::string>> stringVariables;
	};

}