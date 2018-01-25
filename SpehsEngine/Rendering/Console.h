#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <stdint.h>
#include "SpehsEngine/Rendering/ConsoleVariable.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Time.h"
#define CONSOLE_FONT_COLOR 255, 153, 0

/**Console is accessed through these functions.
The console is internally mutex locked and can thus be safely accessed from any thread.
*/
namespace spehs
{
	class BatchManager;
	class InputManager;
	class Polygon;
	class Text;
	namespace time
	{
		struct Time;
	}

	class Console
	{
	public:

		Console(InputManager& inputManager, BatchManager* batchManager = nullptr);
		~Console();

		void open();
		void close();
		bool isOpen();

		//Update cycle
		void update(const time::Time& deltaTime);
		void render(std::string customDebugText = "");

		//Render state
		void setBatchManager(BatchManager* batchManager);
		const BatchManager*  getBatchManager() const;
		BatchManager* getBatchManager();
		void setRenderState(const bool _state);
		bool getRenderState();

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
		void log(const std::string str, const Color color = Color(CONSOLE_FONT_COLOR));
		void log(const char* str, const unsigned length, const Color color = Color(CONSOLE_FONT_COLOR));
		void clearLog();
		bool textEntered();///< Returns true if text was "executed" to console during the last update cycle (text without '/' (command) prefix)
		std::string getTextEntered();///< Returns latest text executed to console

		//Other
		void setPlaneDepth(int16_t depth);
		void setShowStats(const bool show);
		bool getShowStats() const;
		
	private:
		struct LineEntry
		{
			LineEntry() : text(nullptr) {}
			~LineEntry();
			Text* text = nullptr;
			std::string string;
			Color color;
		};
		
		void updateLinePositions();
		void setVariable();
		void executeConsole();
		void createLineText(LineEntry& entry);

		mutable std::recursive_mutex mutex;
		InputManager& inputManager;
		BatchManager* batchManager;
		bool renderState = true;
		bool openState = false;
		bool textExecutedState = false;
		bool showStats = true;
		uint16_t planeDepth = 10000;
		time::Time backspaceTimer = 0;
		int backspaceAcceleration = 0;
		int previousCommandIndex = 0;
		int previousFontSize = 10;
		float visibility = 1.0f;
		Text* consoleText;
		Text* fpsCounter;
		Polygon* backgroundShade;
		std::vector<LineEntry> lines;
		std::string input;
		std::string textExecuted;///< Text executed without '/'
		std::vector<ConsoleCommand> commands;
		std::vector<std::string> consoleWords;
		std::vector<ConsoleVariable<int>> intVariables;
		std::vector<ConsoleVariable<float>> floatVariables;
		std::vector<ConsoleVariable<bool>> boolVariables;
		std::vector<ConsoleVariable<std::string>> stringVariables;
		std::vector<std::string> previousCommands;
	};
}