#pragma once
#include "SpehsEngine/Core/Console.h"
#include "SpehsEngine/Core/LapTimer.h"
#include "SpehsEngine/Input/KeyboardRecorder.h"
#include "boost/signals2.hpp"
#define CONSOLE_FONT_COLOR 255, 153, 0

/**Console is accessed through these functions.
The console is internally mutex locked and can thus be safely accessed from any thread.
*/
namespace se
{
	namespace InputManager
	{
		class InputManager;
	}

	namespace rendering
	{
		class BatchManager;
		class Polygon;
		class Text;
		class Font;
		/*
			Used to visualize a console
		*/
		class ConsoleVisualizer
		{
		public:

			ConsoleVisualizer(Console& console, input::InputManager& inputManager, BatchManager& batchManager);
			~ConsoleVisualizer();

			void open();
			void close();
			bool isOpen();

			//Update cycle
			void update(const time::Time& deltaTime);
			void render(const std::string& customDebugText = "");

			//Font
			void setFont(const std::string& fontPath, const int fontSize);
			void setFont(Font* font);

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

			Console& console;
			input::InputManager& inputManager;
			BatchManager& batchManager;

		private:

			void updateCarotPosition();

			void logCallback(const Console::Line& line);

			size_t getInputLineIndex() const;
			size_t getMaxScrollState() const;

			mutable std::recursive_mutex mutex;

			//Visual state
			bool openState = false;
			bool textExecutedState = false;
			bool showStats = true;
			bool updateLines = true;
			int previousCommandIndex = 0;
			int previousFontSize = 10;
			int scrollState = 0;
			Font* font = nullptr;
			boost::signals2::scoped_connection logConnection;
			time::Time carotTimer = 0;
			time::Time inputTime = 0;//Point of time when the last input was received
			time::LapTimer renderLapTimer;
			std::vector<time::Time> renderLapTimes;

			//Renderables
			Polygon& backgroundShade;//Holds render state and depth
			Text& statsText;
			Text& carotText;
			std::vector<Text*> lines;

			//Input
			input::KeyboardRecorder keyboardRecorder;
			std::vector<std::string> executionHistory;
		};
	}
}