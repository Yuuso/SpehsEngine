#include <algorithm>
#include <iostream>
#include <atomic>
#include <mutex>

#include <boost/bind.hpp>

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/Serializable.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Rendering/Console.h"
#include "SpehsEngine/Rendering/Camera2D.h"
#include "SpehsEngine/Rendering/ConsoleVariable.h"
#include "SpehsEngine/Rendering/Text.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/Polygon.h"

//Misc
#define CONSOLE_COMMANDS_KEPT_IN_MEMORY 10
#define LOG_LINES_KEPT_IN_MEMORY 25
#define CONSOLE_BORDER 5
#define BACKSPACE_INITIAL_INTERVAL 0.5f
#define BACKSPACE_INTERVAL 75
#define FADE_OUT_TIME 2.5f
//FPS
#define FPS_REFRESH_RATE 5


extern std::atomic<int> guiRectangleAllocations;
extern std::atomic<int> guiRectangleDeallocations;
extern std::atomic<int> primitiveAllocations;
extern std::atomic<int> primitiveDeallocations;
extern std::atomic<int> BatchAllocations;
extern std::atomic<int> BatchDeallocations;
extern std::atomic<int> textAllocations;
extern std::atomic<int> textDeallocations;



namespace spehs
{
	Console::Console()
	{
	}

	Console::~Console()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
	}

	//Console variables/commands
	void Console::addVariable(const std::string& str, bool& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		boolVariables.push_back(ConsoleVariable<bool>(str, var));
	}

	void Console::addVariable(const std::string& str, float& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		floatVariables.push_back(ConsoleVariable<float>(str, var));
	}

	void Console::addVariable(const std::string& str, int& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		intVariables.push_back(ConsoleVariable<int>(str, var));
	}

	void Console::addVariable(const std::string& str, std::string& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		stringVariables.push_back(ConsoleVariable<std::string>(str, var));
	}

	void Console::addCommand(const std::string& str, void(*fnc)(void))
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		commands.push_back(ConsoleCommand(str, fnc));
	}

	void Console::addCommand(const std::string& str, void(*fnc)(std::vector<std::string>&))
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		commands.push_back(ConsoleCommand(str, fnc));
	}

	bool Console::removeCommand(const std::string& commandIdentifier)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		for (unsigned i = 0; i < commands.size(); i++)
		{
			if (commands[i]._identifier == commandIdentifier)
			{
				commands.erase(commands.begin() + i);
				return true;
			}
		}
		return false;
	}

	bool Console::removeVariable(const std::string& identifier)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		for (unsigned i = 0; i < intVariables.size(); i++)
		{
			if (intVariables[i]._identifier == identifier)
			{
				intVariables.erase(intVariables.begin() + i);
				return true;
			}
		}
		for (unsigned i = 0; i < floatVariables.size(); i++)
		{
			if (floatVariables[i]._identifier == identifier)
			{
				floatVariables.erase(floatVariables.begin() + i);
				return true;
			}
		}
		for (unsigned i = 0; i < boolVariables.size(); i++)
		{
			if (boolVariables[i]._identifier == identifier)
			{
				boolVariables.erase(boolVariables.begin() + i);
				return true;
			}
		}
		return false;
	}

	void Console::log(const std::string& str, const Color color)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		lines.push_back(Line());
		lines.back().string = str;
		lines.back().color = color;
		logSignal(lines.back());
	}

	void Console::execute(const std::string& str, const Color color)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		log(str, color);
		std::vector<std::string> words;
		getWords(str, words);
		if (words.empty())
			return;
		
		bool foundCommand = false;
		if (words[0] == "set")
		{//Set variables
			if (words.size() >= 3)
			{
				bool isFloat = false;
				for (unsigned i = 0; i < words[2].size(); i++)
					if ((words[2][i] < 48 || words[2][i] > 57) && words[2][i] != 45)//if the character is not "numeric" (number or -)
					{//Value is not numeric

						//Check booleans
						if (words[2] == "true" || words[2] == "false")
						{
							for (unsigned i = 0; i < boolVariables.size(); i++)
								if (words[1] == boolVariables[i]._identifier)
								{
									if (words[2] == "true")
									{
										log("Setting " + boolVariables[i]._identifier + " as true");
										boolVariables[i].set(true);
									}
									else
									{
										log("Setting " + boolVariables[i]._identifier + " as false");
										boolVariables[i].set(false);
									}
									return;
								}
							log("Invalid command!");
							return;
						}

						//Check string variables
						for (unsigned i = 0; i < stringVariables.size(); i++)
						{
							if (stringVariables[i]._identifier == words[1])
							{
								log("Setting " + stringVariables[i]._identifier + " to " + words[2]);
								*stringVariables[i]._variablePtr = words[2];
								return;
							}
						}
						log("Invalid command!");
						return;
					}
					else if (words[2][i] == 46)//Test for period (decimal numbers/aka floats)
					{
						if (isFloat == false)
							isFloat = true;
						else
						{//Two periods -> invalid float
							log("Invalid command!");
							return;
						}
					}

				if (isFloat)
				{
					for (unsigned i = 0; i < floatVariables.size(); i++)
						if (floatVariables[i]._identifier == words[1])
						{
							{
								log("Setting " + floatVariables[i]._identifier + " to " + words[2]);
								floatVariables[i].set(atoi(words[2].c_str()));
								return;
							}
						}
				}
				else
				{
					for (unsigned i = 0; i < intVariables.size(); i++)
						if (intVariables[i]._identifier == words[1])
						{
							{
								log("Setting " + intVariables[i]._identifier + " to " + words[2]);
								intVariables[i].set(atoi(words[2].c_str()));
								return;
							}
						}
				}
				return;
			}
			else
			{
				log("Set failed! Too few parameters in command call");
				return;
			}
		}
		else if (words[0] == "?" || words[0] == "help")
		{//Help
			log("Available variables <set variable value>");
			for (unsigned i = 0; i < boolVariables.size(); i++)
				log("\\\\" + boolVariables[i]._identifier);
			for (unsigned i = 0; i < intVariables.size(); i++)
				log("\\\\" + intVariables[i]._identifier);
			for (unsigned i = 0; i < floatVariables.size(); i++)
				log("\\\\" + floatVariables[i]._identifier);
			for (unsigned i = 0; i < stringVariables.size(); i++)
				log("\\\\" + stringVariables[i]._identifier);
			log("Available commands <command parameter1 parameter2 parameterN>");
			for (unsigned i = 0; i < commands.size(); i++)
				log("\\\\" + commands[i]._identifier);
			return;
		}
		else if (words[0] == "engine")
		{
			if (words.size() > 1 && words[1] == "memory")
			{
				const Color color(204, 204, 204);
				log("-------------------", color);
				log("Spehs Engine select memory allocations:", color);
				log("Remaining allocations / Total (runtime) allocations", color);
				log("GUI Rectangles: " + std::to_string(guiRectangleAllocations - guiRectangleDeallocations) + "/" + std::to_string(guiRectangleAllocations), color);
				log("Primitives: " + std::to_string(primitiveAllocations - primitiveDeallocations) + "/" + std::to_string(primitiveAllocations), color);
				log("PrimitiveBatches: " + std::to_string(BatchAllocations - BatchDeallocations) + "/" + std::to_string(BatchAllocations), color);
				log("-------------------", color);
				return;
			}
		}
		else
		{//Search for command with matching identifier
			for (unsigned i = 0; i < commands.size(); i++)
			{
				if (commands[i]._identifier == words[0])
				{
					if (commands[i]._functionVoid)
						commands[i]._functionVoid();
					else
						commands[i]._functionWords(words);
					return;
				}
			}
		}
		log("Unknown command");
	}

	void Console::clearLog()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		lines.clear();
	}

	void Console::clearVariables()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		boolVariables.clear();
		floatVariables.clear();
		intVariables.clear();
		stringVariables.clear();
	}

	void Console::clearCommands()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		commands.clear();
	}

	const Console::Line& Console::getLine(const size_t index) const
	{
		SPEHS_ASSERT(index < lines.size());
		return lines[index];
	}

	const size_t Console::getLineCount() const
	{
		return lines.size();
	}
	


























































	ConsoleVisualizer::ConsoleVisualizer(Console& _console, InputManager& _inputManager, BatchManager& _batchManager)
		: console(_console)
		, inputManager(_inputManager)
		, batchManager(_batchManager)
		, keyboardRecorder(_inputManager)
		, backgroundShade(*_batchManager.createPolygon(Shape::BUTTON, 0, 1.0f, 1.0f))
		, statsText(*_batchManager.createText())
		, carotText(*_batchManager.createText())
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);

		//Background shade
		backgroundShade.setCameraMatrixState(false);
		backgroundShade.setPosition(0.0f, 0.0f);
		backgroundShade.setColor(spehs::Color(13, 26, 39, 128));
		backgroundShade.setRenderState(true);
		backgroundShade.setAlpha(204);
		
		//Stats text
		statsText.setFont("Fonts/Anonymous.ttf", 12);
		statsText.setColor(Color(255, 77, 0, 217));

		//Carot text
		carotText.setFont("Fonts/Anonymous.ttf", 12);
		carotText.setColor(Color(255, 77, 0, 217));
		carotText.setString("|");

		//Update some values
		setPlaneDepth(0);
		setLineCapacity(10);

		//Connect log connection
		logConnection = console.logSignal.connect(std::bind(&ConsoleVisualizer::logCallback, this, std::placeholders::_1));

		scrollState = getMaxScrollState();
		updateLines = true;
	}

	ConsoleVisualizer::~ConsoleVisualizer()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		backgroundShade.destroy();
		statsText.destroy();
		carotText.destroy();
		for (size_t i = 0; i < lines.size(); i++)
			lines[i]->destroy();
		lines.clear();
	}

	void ConsoleVisualizer::open()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (openState)
			return;
		openState = true;
		updateLines = true;
		lines.back()->setString("");
	}

	void ConsoleVisualizer::close()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!openState)
			return;
		openState = false;
		updateLines = true;
	}

	void ConsoleVisualizer::setRenderState(const bool _state)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		backgroundShade.setRenderState(_state);
	}

	bool ConsoleVisualizer::getRenderState() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return backgroundShade.getRenderState();
	}

	void ConsoleVisualizer::setPlaneDepth(const int16_t depth)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		backgroundShade.setPlaneDepth(depth);
		statsText.setPlaneDepth(depth + 1);
		carotText.setPlaneDepth(depth + 1);
		for (size_t i = 0; i < lines.size(); i++)
			lines[i]->setPlaneDepth(depth + 1);
	}

	int16_t ConsoleVisualizer::getPlaneDepth() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return backgroundShade.getPlaneDepth();
	}

	void ConsoleVisualizer::setShowStats(const bool show)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		showStats = show;
	}

	bool ConsoleVisualizer::getShowStats() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return showStats;
	}

	bool ConsoleVisualizer::isOpen()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return openState;
	}
	
	void ConsoleVisualizer::update(const time::Time& deltaTime)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		
		if (openState)
		{
			//Carot visibility timer
			carotTimer += deltaTime;

			keyboardRecorder.update(deltaTime);
			if (keyboardRecorder.characterInput.size() > 0 || keyboardRecorder.commandInput.size() > 0)
				inputTime = time::now();

			//Append character input
			std::string inputString = lines.back()->getString();
			for (size_t i = 0; i < keyboardRecorder.characterInput.size(); i++)
				inputString += keyboardRecorder.characterInput[i];

			//Command input
			for (size_t i = 0; i < keyboardRecorder.commandInput.size(); i++)
			{
				switch (keyboardRecorder.commandInput[i])
				{
				case KEYBOARD_RETURN:
				{
					const std::string input = lines.back()->getString();
					if (input.size() > 0)
						console.execute(input);
					close();
				}
					break;
				case KEYBOARD_BACKSPACE:
					if (!inputString.empty())
						inputString.pop_back();
					break;
				case KEYBOARD_ESCAPE:
					close();
					break;
				case KEYBOARD_UP:
					//TODO!
					break;
				case KEYBOARD_DOWN:
					//TODO!
					break;
				case KEYBOARD_PAGEUP:
					if (scrollState > 0)
					{
						scrollState--;
						updateLines = true;
					}
					break;
				case KEYBOARD_PAGEDOWN:
					if (scrollState < getMaxScrollState())
					{
						scrollState++;
						updateLines = true;
					}
					break;
				case KEYBOARD_HOME:
					scrollState = 0;
					updateLines = true;
					break;
				case KEYBOARD_END:
					const size_t maxScrollState = getMaxScrollState();
					if (scrollState != maxScrollState)
					{
						scrollState = maxScrollState;
						updateLines = true;
					}
					break;
				}
			}

			if (inputString != lines.back()->getString())
			{
				lines.back()->setString(inputString);
				updateCarotPosition();
			}
		}
		else
		{
			//Console is not open, check opening key combination
			if (inputManager.isKeyDown(KEYBOARD_LCTRL) && inputManager.isKeyDown(KEYBOARD_LALT) && inputManager.isKeyDown(KEYBOARD_BACKSPACE))
			{
				open();
			}
		}
	}

	void ConsoleVisualizer::render(const std::string& customDebugText)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		const bool renderState = getRenderState();
		statsText.setRenderState(renderState && showStats);
		static const time::Time carotInterval = time::fromSeconds(1.0f);
		carotText.setRenderState(renderState && openState && ((((carotTimer.value / carotInterval.value) % 2) == 0) || (time::now() - inputTime) < spehs::time::fromSeconds(1.0f)));
		for (size_t i = 0; i < lines.size(); i++)
			lines[i]->setRenderState(renderState);

		if (renderState)
		{
			//Lines
			if (updateLines)
			{
				SPEHS_ASSERT(lines.size() > 0);

				//Update positions
				const size_t visibleLineCount = std::min(lines.size(), console.getLineCount() + (openState ? 1u : 0u));
				float y = lines.back()->getFontHeight() * (int)visibleLineCount;
				for (size_t i = 0; i < lines.size(); i++)
				{
					y -= (float)lines[i]->getFontHeight();
					lines[i]->setPosition(spehs::vec2(CONSOLE_BORDER, CONSOLE_BORDER + (float)y));
				}

				//Update strings
				const size_t count = openState ? (lines.size() - 1) : lines.size();
				for (size_t i = 0; i < count; i++)
				{
					const size_t consoleLineIndex = std::max(0, scrollState + (int)i + (openState ? 1 : 0));
					if (consoleLineIndex < console.getLineCount())
					{
						const Console::Line& line = console.getLine(consoleLineIndex);
						lines[i]->setString(line.string);
						lines[i]->setColor(line.color);
					}
					else
					{
						lines[i]->setString("");
					}
				}

				//Carot position needs to be updated
				updateCarotPosition();

				updateLines = false;
			}

			//Background shade size
			if (console.getLineCount() == 0 || !openState)
			{
				backgroundShade.resize(0.0f, 0.0f);
			}
			else
			{
				int w = 0.0f;
				int h = 0.0f;
				for (unsigned i = 0; i < lines.size(); i++)
				{
					if (lines[i]->getString().size() > 0)
					{
						w = std::max(w, (int)lines[i]->getTextWidth());
						h += lines[i]->getTextHeight();
					}
				}
				w += CONSOLE_BORDER * 2;
				h += CONSOLE_BORDER * 2;
				if (abs(backgroundShade.getWidth() - w) > 0.9f || abs(backgroundShade.getHeight() - h) > 0.9f)
					backgroundShade.resize(w, h);
			}

			//Status text
			statsText.setString("FPS: TODO\n" + customDebugText);
			statsText.setPosition(spehs::vec2(CONSOLE_BORDER, (float)batchManager.window.getHeight() - statsText.getTextHeight() - CONSOLE_BORDER));
		}
	}

	void ConsoleVisualizer::setLineCapacity(const size_t capacity)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (capacity == lines.size())
			return;

		if (capacity > lines.size())
		{
			const size_t count = capacity - lines.size();
			lines.reserve(capacity);
			for (size_t i = 0; i < count; i++)
			{
				lines.push_back(batchManager.createText());
				lines.back()->setFont("Fonts/Anonymous.ttf", 12);
				lines.back()->setPlaneDepth(backgroundShade.getPlaneDepth() + 1);
				lines.back()->setAlpha(229);
			}
		}
		else if (lines.size() > capacity)
		{
			const size_t count = lines.size() - capacity;
			const size_t backIndex = lines.size() - 1;
			for (size_t i = 0; i < count; i++)
				lines[backIndex - i]->destroy();
			lines.resize(capacity);
		}

		//Update scroll state
		scrollState = std::min(scrollState, (int)getMaxScrollState());

		//Update caret position
		updateCarotPosition();

		//Queue line update
		updateLines = true;
	}

	size_t ConsoleVisualizer::getLineCapacity() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return lines.size();
	}

	void ConsoleVisualizer::logCallback(const Console::Line& line)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		updateLines = true;
		const size_t maxScrollState = getMaxScrollState();
		if (maxScrollState != 0 && scrollState == maxScrollState - 1)
			scrollState++;
	}

	size_t ConsoleVisualizer::getInputLineIndex() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		SPEHS_ASSERT(lines.size() > 0);
		return lines.size() - 1;
	}

	size_t ConsoleVisualizer::getMaxScrollState() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return std::max(0, int(console.getLineCount() - lines.size()));
	}

	void ConsoleVisualizer::updateCarotPosition()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		carotText.setPosition(lines.back()->getPosition() + vec2(lines.back()->getTextWidth() - carotText.getTextWidth() / 2, 0.0f));
	}
}