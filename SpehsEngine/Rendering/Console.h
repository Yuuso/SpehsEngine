#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <stdint.h>
#include <boost/signal.hpp>
#include "SpehsEngine/Rendering/ConsoleVariable.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Input/KeyboardRecorder.h"
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
	class Font;

	class Console
	{
	public:
		struct Line
		{
			std::string string;
			Color color;
		};
	public:

		Console();
		~Console();

		/* Logs string into console memory. String is not considered for execution. */
		void log(const std::string& string, const Color = Color(CONSOLE_FONT_COLOR));
		/* Logs string into console memory. String is evaluated for execution. */
		void execute(const std::string& string, const Color = Color(CONSOLE_FONT_COLOR));

		//Line access
		const Line& getLine(const size_t index) const;
		const size_t getLineCount() const;
		
		//Variables, commands
		void addVariable(const std::string& identifier, bool& var);
		void addVariable(const std::string& identifier, float& var);
		void addVariable(const std::string& identifier, int& var);
		void addVariable(const std::string& identifier, std::string& var);
		void addCommand(const std::string& identifier, void(*fnc)(void));
		void addCommand(const std::string& identifier, void(*fnc)(std::vector<std::string>&));
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

	/*
		Used to visualize a console
	*/
	class ConsoleVisualizer
	{
	public:

		ConsoleVisualizer(Console& console, InputManager& inputManager, BatchManager& batchManager);
		~ConsoleVisualizer();

		void open();
		void close();
		bool isOpen();

		//Update cycle
		void update(const time::Time& deltaTime);
		void render(const std::string& customDebugText = "");

		//Font
		void setFont(const std::string& fontPath, const int fontSize);
		void setFont(const spehs::Font& font);

		//Render state
		void setRenderState(const bool state);
		bool getRenderState() const;

		//Depth
		void setPlaneDepth(const int16_t depth);
		int16_t getPlaneDepth() const;

		//Show stats
		void setShowStats(const bool show);
		bool getShowStats() const;

		//Line capacity
		void setLineCapacity(const size_t capacity);
		size_t getLineCapacity() const;
		
	private:
		
		void updateCarotPosition();

		void logCallback(const Console::Line& line);

		size_t getInputLineIndex() const;
		size_t getMaxScrollState() const;		

		mutable std::recursive_mutex mutex;
		Console& console;
		InputManager& inputManager;
		BatchManager& batchManager;

		//Visual state
		bool openState = false;
		bool textExecutedState = false;
		bool showStats = true;
		bool updateLines = true;
		int previousCommandIndex = 0;
		int previousFontSize = 10;
		int scrollState = 0;
		Font* font = nullptr;
		boost::signals::scoped_connection logConnection;
		time::Time carotTimer = 0;
		time::Time inputTime = 0;//Point of time when the last input was received

		//Renderables
		Polygon& backgroundShade;//Holds render state and depth
		Text& statsText;
		Text& carotText;
		std::vector<Text*> lines;

		//Input
		KeyboardRecorder keyboardRecorder;
		std::vector<std::string> executionHistory;
	};
}