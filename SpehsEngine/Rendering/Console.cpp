#include <algorithm>
#include <iostream>
#include <atomic>
#include <mutex>

#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/BitwiseOperations.h"
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
	Console::LineEntry::~LineEntry()
	{
		if (text)
			text->destroy();
		text = nullptr;
	}

	Console::Console(InputManager& _inputManager, BatchManager* _batchManager)
		: inputManager(_inputManager)
		, batchManager(nullptr)
		, backgroundShade(nullptr)
		, fpsCounter(nullptr)
		, consoleText(nullptr)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);

		if (batchManager)
			setBatchManager(_batchManager);
		setPlaneDepth(planeDepth + 1);
	}

	Console::~Console()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);

		if (backgroundShade)
			backgroundShade->destroy();
		backgroundShade = nullptr;

		if (fpsCounter)
			fpsCounter->destroy();
		fpsCounter = nullptr;

		if (consoleText)
			consoleText->destroy();
		consoleText = nullptr;

		clearLog();
	}

	void Console::open()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (openState)
			return;
		openState = true;
		consoleText->setRenderState(renderState);
		updateLinePositions();
	}

	void Console::close()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!openState)
			return;
		openState = false;
		consoleText->setRenderState(false);
		updateLinePositions();
		input.clear();
	}

	void Console::setRenderState(const bool _state)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (_state != renderState)
		{
			renderState = _state;
			if (batchManager)
			{
				fpsCounter->setRenderState(_state);
				backgroundShade->setRenderState(_state);
				for (size_t i = 0; i < lines.size(); i++)
					lines[i].text->setRenderState(_state);
			}
		}
	}

	bool Console::getRenderState()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return renderState;
	}

	bool Console::isOpen()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return openState;
	}

	bool Console::textEntered()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return textExecutedState;
	}

	std::string Console::getTextEntered()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return textExecuted;
	}

	void Console::update(const time::Time& deltaTime)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);

		//Reset the text executed state
		textExecutedState = false;
		
		//Remove lines if needed
		while (lines.size() >= LOG_LINES_KEPT_IN_MEMORY)
		{
			lines.erase(lines.begin());
		}
		
		if (openState)
		{
			//Increase visibility
			if (visibility < 1.0f)
			{
				visibility += deltaTime.asSeconds() * 5.0f;
				if (visibility > 1.0f)
					visibility = 1.0f;
			}

			////Receiving input
			bool inputReceived = false;
			//Alphabet
			unsigned char capital = 0;
			if (inputManager.isKeyDown(KEYBOARD_LSHIFT) || inputManager.isKeyDown(KEYBOARD_RSHIFT))
				capital = 32;
			for (unsigned char i = 61; i <= 122; i++)
			{
				if (inputManager.isKeyPressed(i))
				{
					input += i - capital;
					inputReceived = true;
				}
			}

			//Numbers
			if (!inputManager.isKeyDown(KEYBOARD_LSHIFT) && !inputManager.isKeyDown(KEYBOARD_RSHIFT))
			{
				for (unsigned char i = 48; i <= 57; i++)
				{
					if (inputManager.isKeyPressed(i))
					{
						input += i;
						inputReceived = true;
					}
				}
			}
			else
			{
				//Special characters
				for (unsigned char i = 48; i <= 59; i++)
				{
					if (inputManager.isKeyPressed(i))
					{
						input += i - 10;
						inputReceived = true;
					}
				}
			}

			//Minus
			if (inputManager.isKeyPressed(45))
			{
				input += 45;
				inputReceived = true;
			}
			//Period
			if (inputManager.isKeyPressed(46))
			{
				input += 46;
				inputReceived = true;
			}
			//Space
			if (inputManager.isKeyPressed(32))
			{
				input += 32;
				inputReceived = true;
			}
			//Backspace
			if (inputManager.isKeyDown(8))
			{
				if (backspaceTimer <= time::Time::zero && input.size() > 0)
				{
					if (inputManager.isKeyDown(KEYBOARD_LCTRL) || inputManager.isKeyDown(KEYBOARD_RCTRL))
					{//Erase a word
						if (input.back() == ' ')
							input.pop_back();
						else
						{//Erase until space or empty
							while (input.size() > 0 && input.back() != ' ')
								input.pop_back();
							if (input.size() == 1 && input.back() == ' ')
								input.pop_back();
							backspaceTimer = time::fromSeconds(BACKSPACE_INITIAL_INTERVAL);
						}
					}
					else
					{//Erase one character
						input.pop_back();
						if (inputManager.isKeyPressed(8))
						{
							backspaceTimer = time::fromSeconds(BACKSPACE_INITIAL_INTERVAL);
						}
						else
						{
							backspaceTimer = time::fromMilliseconds(BACKSPACE_INTERVAL - backspaceAcceleration);
							backspaceAcceleration += 3;
						}
					}
					inputReceived = true;
				}
				else
				{
					backspaceTimer -= deltaTime;
				}
			}
			else
			{
				backspaceTimer = time::Time::zero;
				backspaceAcceleration = 0;
			}

			//Enter
			if (inputManager.isKeyPressed(KEYBOARD_RETURN))
			{
				executeConsole();
				close();
				inputReceived = true;
			}

			//Escape
			if (inputManager.isKeyPressed(KEYBOARD_ESCAPE))
			{
				close();
				inputReceived = true;
			}

			//Previous commands
			if (inputManager.isKeyPressed(KEYBOARD_UP) && previousCommands.size() > 0)
			{
				if (previousCommandIndex == -1)
					previousCommandIndex = previousCommands.size() - 1;
				else if (--previousCommandIndex < 0)
					previousCommandIndex = previousCommands.size() - 1;
				input = previousCommands[previousCommandIndex];
				inputReceived = true;
			}
			if (inputManager.isKeyPressed(KEYBOARD_DOWN) && previousCommands.size() > 0)
			{
				if (previousCommandIndex == -1)
					previousCommandIndex = 0;
				else if (++previousCommandIndex >= previousCommands.size())
					previousCommandIndex = 0;
				input = previousCommands[previousCommandIndex];
				inputReceived = true;
			}


			if (inputReceived)
			{
				consoleText->setString('>' + input + '<');
			}
		}
		else
		{
			//Console is not open, check opening key combination
			if (inputManager.isKeyDown(KEYBOARD_LCTRL) && inputManager.isKeyDown(KEYBOARD_LALT) && inputManager.isKeyDown(KEYBOARD_BACKSPACE))
			{
				open();
				input = "";
			}

			//Decrease visibility over time
			if (visibility > 0.0f)
			{
				visibility -= deltaTime.asSeconds() / FADE_OUT_TIME;
				if (visibility < 0.0f)
					visibility = 0.0f;
			}
		}
	}

	void Console::render(std::string customDebugText)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!batchManager || renderState == false || openState == false)
		{
			return;
		}
		
		//Create rendered elements if they dont already exist
		if (batchManager)
		{
			//Console background shade
			if (openState || (lines.size() > 0 && lines.front().text && lines.front().text->getRenderState()))
			{
				if (!backgroundShade)
				{
					backgroundShade = batchManager->createPolygon(Shape::BUTTON, planeDepth, 1.0f, 1.0f);
					backgroundShade->setCameraMatrixState(false);
					backgroundShade->setPosition(0.0f, 0.0f);
					backgroundShade->setColor(spehs::Color(13, 26, 39, 128));
				}
				backgroundShade->setRenderState(true);
				backgroundShade->setAlpha(int(visibility * 204));
				int w = 0.0f, h = 0.0f;
				if (consoleText->getRenderState())
				{
					w = consoleText->getTextWidth();
					h = consoleText->getTextHeight();
				}
				for (unsigned i = 0; i < lines.size(); i++)
				{
					w = std::max(w, (int)lines[i].text->getTextWidth());
					h += lines[i].text->getTextHeight();
				}
				w += CONSOLE_BORDER * 2;
				h += CONSOLE_BORDER * 2;
				if (abs(backgroundShade->getWidth() - w) > 0.9f || abs(backgroundShade->getHeight() - h) > 0.9f)
					backgroundShade->resize(w, h);
			}
			else
				backgroundShade->setRenderState(false);

			//FPS counter
			if (!fpsCounter)
			{
				fpsCounter = batchManager->createText(10000);
				fpsCounter->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
				fpsCounter->setColor(Color(255, 77, 0, 217));
			}
			fpsCounter->setString("FPS: TODO\n" + customDebugText);
			fpsCounter->setPosition(spehs::vec2(CONSOLE_BORDER, (float)batchManager->window.getHeight() - fpsCounter->getTextHeight() - CONSOLE_BORDER));
			fpsCounter->setRenderState(showStats);

			//Console text
			if (!consoleText)
			{
				consoleText = batchManager->createText();
				consoleText->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
				consoleText->setColor(Color(255, 153, 0, spehs::ApplicationData::consoleTextAlpha));
				consoleText->setPosition(spehs::vec2(CONSOLE_BORDER, CONSOLE_BORDER));
				consoleText->setString("><");
				consoleText->setPlaneDepth(planeDepth);
			}
			consoleText->setRenderState(openState);
			consoleText->setAlpha(int(visibility * (spehs::ApplicationData::consoleTextAlpha)));

			for (size_t i = 0; i < lines.size(); i++)
			{
				if (lines[i].text == nullptr)
				{
					lines[i].text = batchManager->createText();
					lines[i].text->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
					lines[i].text->setAlpha(spehs::ApplicationData::consoleTextAlpha);
					lines[i].text->setPlaneDepth(planeDepth);
					lines[i].text->setColor(lines[i].color);
					lines[i].text->setString(lines[i].string);
					lines[i].text->setRenderState(renderState);
				}
				lines[i].text->setAlpha(int(visibility * (spehs::ApplicationData::consoleTextAlpha)));
			}
		}

	}

	//Console variables/commands
	void Console::addVariable(std::string str, bool& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		boolVariables.push_back(ConsoleVariable<bool>(str, var));
	}

	void Console::addVariable(std::string str, float& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		floatVariables.push_back(ConsoleVariable<float>(str, var));
	}

	void Console::addVariable(std::string str, int& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		intVariables.push_back(ConsoleVariable<int>(str, var));
	}

	void Console::addVariable(std::string str, std::string& var)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		stringVariables.push_back(ConsoleVariable<std::string>(str, var));
	}

	void Console::addCommand(std::string str, void(*fnc)(void))
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		commands.push_back(ConsoleCommand(str, fnc));
	}

	void Console::addCommand(std::string str, void(*fnc)(std::vector<std::string>&))
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		commands.push_back(ConsoleCommand(str, fnc));
	}

	bool Console::removeCommand(std::string commandIdentifier)
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

	bool Console::removeVariable(std::string identifier)
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

	//Console logging
	void Console::log(const std::string str, const Color color)
	{
		log(&str[0], str.size(), color);
	}

	void Console::log(const char* str, const unsigned length, const Color color)
	{
		//Validate string characters
		std::string string(str, length);
		for (unsigned i = 0; i < length; i++)
		{
			if (string[i] < 32 && string[i] != '\n')
				string[i] = 32;
		}

		std::lock_guard<std::recursive_mutex> lock(mutex);
		lines.push_back(LineEntry());
		lines.back().string = string;
		lines.back().color = color;
	}

	void Console::clearLog()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		lines.clear();
	}

	void Console::setPlaneDepth(int16_t depth)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		planeDepth = depth;
		if (consoleText)
			consoleText->setPlaneDepth(depth);
		for (unsigned i = 0; i < lines.size(); i++)
		{
			if (lines[i].text)
				lines[i].text->setPlaneDepth(depth);
		}
	}

	void Console::setShowStats(const bool show)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		showStats = show;
	}

	bool Console::getShowStats() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return showStats;
	}

	void Console::updateLinePositions()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (lines.size() == 0 || !batchManager)
			return;

		int y = 0;
		if (openState)
			y += consoleText->getTextHeight();
		for (int i = int(lines.size()) - 1; i >= 0; i--)
		{
			lines[i].text->setPosition(spehs::vec2(CONSOLE_BORDER, CONSOLE_BORDER + y));
			y += lines[i].text->getTextHeight();
		}
	}

	void Console::executeConsole()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (input.size() == 0)
			return;

		//Record command
		for (unsigned i = 0; i < previousCommands.size(); i++)
		{//Erase if command already recorded
			if (previousCommands[i] == input)
			{
				previousCommands.erase(previousCommands.begin() + i);
				break;
			}
		}
		previousCommands.push_back(input);
		if (previousCommands.size() > CONSOLE_COMMANDS_KEPT_IN_MEMORY)
			previousCommands.erase(previousCommands.begin());
		previousCommandIndex = -1;

		if (input[0] != '/')
		{//Plain text, not command
			textExecutedState = true;
			consoleText->setString("><");
			textExecuted = input;
			input.clear();
			return;
		}
		input.erase(input.begin());///Erase '/'


		//Record each word as a different element in console words vector
		bool foundCommand = false;
		consoleWords.clear();
		consoleWords.push_back(std::string(""));
		for (unsigned i = 0; i < input.size(); i++)
		{
			if (input[i] != '\0')
			{
				if (input[i] == ' ')
					consoleWords.push_back(std::string(""));
				else//Record character
					consoleWords.back() += input[i];
			}
			else
			{
				break;
			}
		}


		if (consoleWords[0] == "set")
		{//Set variables
			if (consoleWords.size() >= 3)
			{
				setVariable();
				foundCommand = true;
			}
			else
			{
				log("Set failed! Too few parameters in command call");
				return;
			}
		}
		else if (consoleWords[0] == "?" || consoleWords[0] == "help")
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
		else if (consoleWords[0] == "engine")
		{
			if (consoleWords.size() > 1 && consoleWords[1] == "memory")
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
			return;
		}
		else
		{//Search for command with matching identifier
			for (unsigned i = 0; i < commands.size(); i++)
			{
				if (commands[i]._identifier == consoleWords[0])
				{
					if (commands[i]._functionVoid)
						commands[i]._functionVoid();
					else
						commands[i]._functionWords(consoleWords);
					foundCommand = true;
				}
			}
		}

		input = "";
		consoleText->setString("><");
		if (!foundCommand)
		{
			log("Unknown command");
			return;
		}
	}

	void Console::setBatchManager(BatchManager* _batchManager)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (batchManager == _batchManager)
			return;

		//Remove renderables from previous batch manager
		if (batchManager)
		{
			backgroundShade->destroy();
			backgroundShade = nullptr;
			fpsCounter->destroy();
			fpsCounter = nullptr;
			consoleText->destroy();
			consoleText = nullptr;
			for (size_t i = 0; i < lines.size(); i++)
			{
				lines[i].text->destroy();
				lines[i].text = nullptr;
			}
		}
		batchManager = _batchManager;
	}

	const BatchManager* Console::getBatchManager() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return batchManager;
	}

	BatchManager* Console::getBatchManager()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return batchManager;
	}
	
	void Console::setVariable()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		bool isFloat = false;
		for (unsigned i = 0; i < consoleWords[2].size(); i++)
		if ((consoleWords[2][i] < 48 || consoleWords[2][i] > 57) && consoleWords[2][i] != 45)//if the character is not "numeric" (number or -)
		{//Value is not numeric

			//Check booleans
			if (consoleWords[2] == "true" || consoleWords[2] == "false")
			{
				for (unsigned i = 0; i < boolVariables.size(); i++)
				if (consoleWords[1] == boolVariables[i]._identifier)
				{
					if (consoleWords[2] == "true")
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
				if (stringVariables[i]._identifier == consoleWords[1])
				{
					log("Setting " + stringVariables[i]._identifier + " to " + consoleWords[2]);
					*stringVariables[i]._variablePtr = consoleWords[2];
					return;
				}
			}
			log("Invalid command!");
			return;
		}
		else if (consoleWords[2][i] == 46)//Test for period (decimal numbers/aka floats)
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
			if (floatVariables[i]._identifier == consoleWords[1])
			{
				{
					log("Setting " + floatVariables[i]._identifier + " to " + consoleWords[2]);
					floatVariables[i].set(atoi(consoleWords[2].c_str()));
					return;
				}
			}
		}
		else
		{
			for (unsigned i = 0; i < intVariables.size(); i++)
			if (intVariables[i]._identifier == consoleWords[1])
			{
				{
					log("Setting " + intVariables[i]._identifier + " to " + consoleWords[2]);
					intVariables[i].set(atoi(consoleWords[2].c_str()));
					return;
				}
			}
		}
	}
}