#include <algorithm>
#include <iostream>
#include <atomic>
#include <mutex>

#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Serializable.h"
#include "SpehsEngine/Core/Time.h"
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
typedef std::lock_guard<std::recursive_mutex> LockGuardRecursive;



namespace spehs
{
	Console::Console(BatchManager& _batchManager, InputManager& _inputManager)
		: batchManager(_batchManager)
		, inputManager(_inputManager)
	{
		LockGuardRecursive regionLock(mutex);
		backgroundShade = batchManager.createPolygon(Shape::BUTTON, planeDepth, 1.0f, 1.0f);
		backgroundShade->setCameraMatrixState(false);
		backgroundShade->setPosition(0.0f, 0.0f);
		backgroundShade->setColor(spehs::Color(13, 26, 39, 128));

		fpsCounter = batchManager.createText(10000);
		fpsCounter->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
		fpsCounter->setColor(Color(255, 77, 0, 217));
		fpsCounter->setString("FPS:0123456789\nDraw calls:0123456789\nVertices:0123456789");
		fpsCounter->setPosition(spehs::vec2(5, (float)batchManager.window.getHeight() - fpsCounter->getTextHeight()));

		consoleText = batchManager.createText();
		consoleText->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
		consoleText->setColor(Color(255, 153, 0, spehs::ApplicationData::consoleTextAlpha));
		consoleText->setPosition(spehs::vec2(CONSOLE_BORDER, CONSOLE_BORDER));
		consoleText->setString("><");
		consoleText->setRenderState(checkState(CONSOLE_OPEN_BIT));
		setPlaneDepth(planeDepth + 1);
	}

	Console::~Console()
	{
		LockGuardRecursive regionLock(mutex);

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
		LockGuardRecursive regionLock(mutex);
		if (checkState(CONSOLE_OPEN_BIT))
			return;
		enableState(CONSOLE_OPEN_BIT);
		consoleText->setRenderState(true);
		updateLinePositions();
	}

	void Console::close()
	{
		LockGuardRecursive regionLock(mutex);
		if (!checkBit(state, CONSOLE_OPEN_BIT))
			return;
		disableBit(state, CONSOLE_OPEN_BIT);
		consoleText->setRenderState(false);
		updateLinePositions();
		input.clear();
	}

	void Console::setRenderState(const bool _state)
	{
		if (_state)
			enableState(CONSOLE_RENDER_STATE_BIT);
		else
			disableState(CONSOLE_RENDER_STATE_BIT);
	}

	bool Console::getRenderState()
	{
		return checkState(CONSOLE_RENDER_STATE_BIT);
	}

	bool Console::isOpen()
	{
		return checkState(CONSOLE_OPEN_BIT);
	}

	bool Console::checkState(uint16_t bits)
	{
		LockGuardRecursive regionLock(mutex);
		return checkBit(state, bits);
	}

	void Console::enableState(uint16_t bits)
	{
		mutex.lock();
		enableBit(state, bits);
		mutex.unlock();
	}

	void Console::disableState(uint16_t bits)
	{
		mutex.lock();
		disableBit(state, bits);
		mutex.unlock();
	}

	bool Console::textEntered()
	{
		LockGuardRecursive regionLock(mutex);
		return checkState(CONSOLE_TEXT_EXECUTED_BIT);
	}

	std::string Console::getTextEntered()
	{
		LockGuardRecursive regionLock(mutex);
		return textExecuted;
	}

	void Console::update(const time::Time& deltaTime)
	{
		LockGuardRecursive regionLock(mutex);
			
		//Process new lines entered during the cycle
		for (unsigned i = 0; i < newLines.size(); i++)
		{
			if (lines.size() >= LOG_LINES_KEPT_IN_MEMORY)
			{
				lines.front()->destroy();
				lines.erase(lines.begin());
			}
			lines.push_back(batchManager.createText(planeDepth));
			lines.back()->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
			lines.back()->setColor(newLines[i].second);
			lines.back()->setAlpha(spehs::ApplicationData::consoleTextAlpha);
			lines.back()->setString(&newLines[i].first[0], newLines[i].first.size());
			visibility = 1.0f;
			for (unsigned i = 0; i < lines.size(); i++)
				lines[i]->setRenderState(true);

			updateLinePositions();
		}
		newLines.clear();

		disableState(CONSOLE_TEXT_EXECUTED_BIT);
		if (visibility > 0.0f)
		{
			//Update console font size if needed
			if (previousFontSize != spehs::ApplicationData::consoleTextSize)
			{
				fpsCounter->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
				consoleText->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
				for (unsigned i = 0; i < lines.size(); i++)
					lines[i]->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
				previousFontSize = spehs::ApplicationData::consoleTextSize;

			}

			if (!isOpen())
			{
				visibility -= deltaTime.asSeconds() / FADE_OUT_TIME;
				if (visibility <= 0.0f)
				{
					for (unsigned i = 0; i < lines.size(); i++)
						lines[i]->setRenderState(false);
				}
			}
		}

		if (!isOpen())
		{
			//Console is not open, check opening key combination
			if (inputManager.isKeyDown(KEYBOARD_LCTRL) && inputManager.isKeyDown(KEYBOARD_LALT) && inputManager.isKeyDown(KEYBOARD_BACKSPACE))
			{
				open();
				input = "";
			}
		}
		else
		{
			if (visibility < 1.0f)
			{
				if (visibility <= 0.0f)
				{
					for (unsigned i = 0; i < lines.size(); i++)
						lines[i]->setRenderState(true);
				}
				visibility += deltaTime.asSeconds() * 5.0f;
				if (visibility > 1.0f)
					visibility = 1.0f;
			}

			//Console is open
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
	}

	void Console::render(std::string customDebugText)
	{
		LockGuardRecursive regionLock(mutex);

		if (!checkState(CONSOLE_RENDER_STATE_BIT) && !isOpen())
		{
			drawCalls = 0;
			vertexDrawCount = 0;
			return;
		}

		if (spehs::ApplicationData::showFps)
		{
			fpsCounter->setRenderState(true);

			static int frameCounter = 0;
			if (++frameCounter >= FPS_REFRESH_RATE)
			{
				fpsCounter->setString("Draw calls: " + std::to_string(drawCalls) + "\nVertices: " + std::to_string(vertexDrawCount) + "\n" + customDebugText);
				fpsCounter->setPosition(spehs::vec2(CONSOLE_BORDER, batchManager.window.getHeight() - fpsCounter->getTextHeight()));
				frameCounter = 0;
			}
		}
		else
			fpsCounter->setRenderState(false);

		drawCalls = 0;
		vertexDrawCount = 0;

		batchManager.render();

		//Render lines
		for (unsigned i = 0; i < lines.size(); i++)
			lines[i]->setAlpha(int(visibility * (spehs::ApplicationData::consoleTextAlpha)));

		//Console text
		consoleText->setAlpha(int(visibility * (spehs::ApplicationData::consoleTextAlpha)));

		//Console background shade
		if (consoleText->getRenderState() || (lines.size() > 0 && lines.front()->getRenderState()))
		{
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
				w = std::max(w, (int)lines[i]->getTextWidth());
				h += lines[i]->getTextHeight();
			}
			w += CONSOLE_BORDER * 2;
			h += CONSOLE_BORDER * 2;
			if (abs(backgroundShade->getWidth() - w) > 0.9f || abs(backgroundShade->getHeight() - h) > 0.9f)
				backgroundShade->resize(w, h);
		}
		else
			backgroundShade->setRenderState(false);
	}

	//Console variables/commands
	void Console::addVariable(std::string str, bool& var)
	{
		LockGuardRecursive regionLock(mutex);
		boolVariables.push_back(ConsoleVariable<bool>(str, var));
	}

	void Console::addVariable(std::string str, float& var)
	{
		LockGuardRecursive regionLock(mutex);
		floatVariables.push_back(ConsoleVariable<float>(str, var));
	}

	void Console::addVariable(std::string str, int& var)
	{
		LockGuardRecursive regionLock(mutex);
		intVariables.push_back(ConsoleVariable<int>(str, var));
	}

	void Console::addVariable(std::string str, std::string& var)
	{
		LockGuardRecursive regionLock(mutex);
		stringVariables.push_back(ConsoleVariable<std::string>(str, var));
	}

	void Console::addCommand(std::string str, void(*fnc)(void))
	{
		LockGuardRecursive regionLock(mutex);
		commands.push_back(ConsoleCommand(str, fnc));
	}

	void Console::addCommand(std::string str, void(*fnc)(std::vector<std::string>&))
	{
		LockGuardRecursive regionLock(mutex);
		commands.push_back(ConsoleCommand(str, fnc));
	}

	bool Console::removeCommand(std::string commandIdentifier)
	{
		LockGuardRecursive regionLock(mutex);
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
		LockGuardRecursive regionLock(mutex);
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
		LockGuardRecursive regionLock(mutex);
		boolVariables.clear();
		floatVariables.clear();
		intVariables.clear();
		stringVariables.clear();
	}

	void Console::clearCommands()
	{
		LockGuardRecursive regionLock(mutex);
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

		LockGuardRecursive regionLock(mutex);
		newLines.push_back(std::make_pair(string, color));
	}

	void Console::clearLog()
	{
		LockGuardRecursive regionLock(mutex);
		while (!lines.empty())
		{
			lines.back()->destroy();
			lines.pop_back();
		}
	}

	void Console::setPlaneDepth(int16_t depth)
	{
		planeDepth = depth;
		consoleText->setPlaneDepth(depth);
		for (unsigned i = 0; i < lines.size(); i++)
			lines[i]->setPlaneDepth(depth);
	}

	void Console::updateLinePositions()
	{
		LockGuardRecursive regionLock(mutex);
		if (lines.size() == 0)
			return;

		int y = 0;
		if (isOpen())
			y += consoleText->getTextHeight();
		for (int i = int(lines.size()) - 1; i >= 0; i--)
		{
			lines[i]->setPosition(spehs::vec2(CONSOLE_BORDER, CONSOLE_BORDER + y));
			y += lines[i]->getTextHeight();
		}
	}

	void Console::executeConsole()
	{
		LockGuardRecursive regionLock(mutex);
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
			enableState(CONSOLE_TEXT_EXECUTED_BIT);
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
				static const Color color(204, 204, 204);
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

	void Console::setVariable()
	{
		LockGuardRecursive regionLock(mutex);
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