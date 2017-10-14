#include <algorithm>
#include <iostream>
#include <atomic>
#include <mutex>

#include "BitwiseOperations.h"
#include "InputManager.h"
#include "ApplicationData.h"
#include "Time.h"
#include "Console.h"
#include "Camera2D.h"
#include "ConsoleVariable.h"
#include "Text.h"
#include "BatchManager.h"
#include "Polygon.h"
#include "Serializable.h"

//State
#define CONSOLE_INITIALIZED_BIT				0x0001
#define CONSOLE_OPEN_BIT					0x0002
#define CONSOLE_TEXT_EXECUTED_BIT			0x0004
#define CONSOLE_RENDER_STATE_BIT			0x0008
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
	extern std::atomic<int> serializableAllocations;
	extern std::atomic<int> serializableDeallocations;
	namespace console
	{
		//Static console data
		static int16_t state = CONSOLE_RENDER_STATE_BIT;
		static uint16_t planeDepth = 10000;
		static time::Time backspaceTimer = 0;
		static int backspaceAcceleration = 0;
		static int previousCommandIndex = 0;
		static int previousFontSize = 10;
		static float visibility = 1.0f;
		unsigned long drawCalls;
		unsigned long vertexDrawCount;
		static Text* consoleText;
		spehs::Text* fpsCounter;
		spehs::Polygon* backgroundShade;
		static std::recursive_mutex consoleMutex;
		static std::string input;
		static std::string textExecuted;///< Text executed without '/'
		static std::vector<Text*> lines;
		static std::vector<std::pair<std::string, Color>> newLines;
		static std::vector<ConsoleCommand> commands;
		static std::vector<std::string> consoleWords;
		static std::vector<ConsoleVariable<int>> intVariables;
		static std::vector<ConsoleVariable<float>> floatVariables;
		static std::vector<ConsoleVariable<bool>> boolVariables;
		static std::vector<ConsoleVariable<std::string>> stringVariables;
		static std::vector<std::string> previousCommands;
		static bool checkState(uint16_t bits);
		static void enableState(uint16_t bits);
		static void disableState(uint16_t bits);

		static Camera2D* consoleCamera;
		static BatchManager* consoleBatchManager;

		//Local methods
		void updateLinePositions();
		void setVariable();
		void executeConsole();


		//Console state
		int initialize()
		{
			LockGuardRecursive regionLock(consoleMutex);
			if (checkState(CONSOLE_INITIALIZED_BIT))
			{
				log("Console already initialized!");
				return 0;
			}

			consoleCamera = new Camera2D();
			consoleBatchManager = new BatchManager(consoleCamera, "console");

			backgroundShade = consoleBatchManager->createPolygon(Shape::BUTTON, planeDepth, 1.0f, 1.0f);
			if (!backgroundShade)
			{
				std::cout << "\nInitialization failed! Failed to create backgroundShade!";
				return false;
			}
			backgroundShade->setCameraMatrixState(false);
			backgroundShade->setPosition(0.0f, 0.0f);
			backgroundShade->setColor(spehs::Color(0.05f, 0.1f, 0.15f, 0.5f));

			fpsCounter = consoleBatchManager->createText(10000);
			if (!fpsCounter)
			{
				std::cout << "\nInitialization failed! Failed to create fpsCounter!";
				return false;
			}
			fpsCounter->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
			fpsCounter->setColor(Color(1.0f, 0.3f, 0.0f, 0.85f));
			fpsCounter->setString("FPS:0123456789\nDraw calls:0123456789\nVertices:0123456789");
			fpsCounter->setPosition(spehs::vec2(5, spehs::ApplicationData::getWindowHeight() - fpsCounter->getTextHeight()));

			consoleText = consoleBatchManager->createText();
			consoleText->setFont(spehs::ApplicationData::GUITextFontPath, spehs::ApplicationData::consoleTextSize);
			consoleText->setColor(Color(1.0f, 0.6f, 0.0f, spehs::ApplicationData::consoleTextAlpha));
			consoleText->setPosition(spehs::vec2(CONSOLE_BORDER, CONSOLE_BORDER));
			consoleText->setString("><");
			consoleText->setRenderState(checkState(CONSOLE_OPEN_BIT));
			setPlaneDepth(planeDepth + 1);

			enableState(CONSOLE_INITIALIZED_BIT);
			log("Console initialized");
			return 0;
		}
		void unitialize()
		{
			LockGuardRecursive regionLock(consoleMutex);
			if (!checkState(CONSOLE_INITIALIZED_BIT))
			{
				std::cout << "\nSpehsEngine::console already uninitialized!";
				return;
			}

			if (backgroundShade)
				backgroundShade->destroy();
			backgroundShade = nullptr;

			if (fpsCounter)
				fpsCounter->destroy();
			fpsCounter = nullptr;

			if (consoleText)
				consoleText->destroy();
			consoleText = nullptr;

			delete consoleBatchManager;
			delete consoleCamera;

			clearLog();
			disableState(CONSOLE_INITIALIZED_BIT);
		}
		void open()
		{
			LockGuardRecursive regionLock(consoleMutex);
			if (checkState(CONSOLE_OPEN_BIT))
				return;
			enableState(CONSOLE_OPEN_BIT);
			consoleText->setRenderState(true);
			updateLinePositions();
		}
		void close()
		{
			LockGuardRecursive regionLock(consoleMutex);
			if (!checkBit(state, CONSOLE_OPEN_BIT))
				return;
			disableBit(state, CONSOLE_OPEN_BIT);
			consoleText->setRenderState(false);
			updateLinePositions();
			input.clear();
		}
		void setRenderState(const bool _state)
		{
			if (_state)
				enableState(CONSOLE_RENDER_STATE_BIT);
			else
				disableState(CONSOLE_RENDER_STATE_BIT);
		}
		bool getRenderState()
		{
			return checkState(CONSOLE_RENDER_STATE_BIT);
		}
		bool isOpen()
		{
			return checkState(CONSOLE_OPEN_BIT);
		}
		bool checkState(uint16_t bits)
		{
			LockGuardRecursive regionLock(consoleMutex);
			return checkBit(state, bits);
		}
		void enableState(uint16_t bits)
		{
			consoleMutex.lock();
			enableBit(state, bits);
			consoleMutex.unlock();
		}
		void disableState(uint16_t bits)
		{
			consoleMutex.lock();
			disableBit(state, bits);
			consoleMutex.unlock();
		}
		bool textEntered()
		{
			LockGuardRecursive regionLock(consoleMutex);
			return checkState(CONSOLE_TEXT_EXECUTED_BIT);
		}
		std::string getTextEntered()
		{
			LockGuardRecursive regionLock(consoleMutex);
			return textExecuted;
		}
		void update(const time::Time& deltaTime)
		{
			LockGuardRecursive regionLock(consoleMutex);
			
			//Process new lines entered during the cycle
			for (unsigned i = 0; i < newLines.size(); i++)
			{
				if (lines.size() >= LOG_LINES_KEPT_IN_MEMORY)
				{
					lines.front()->destroy();
					lines.erase(lines.begin());
				}
				lines.push_back(consoleBatchManager->createText(planeDepth));
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
				if (inputManager->isKeyDown(KEYBOARD_LCTRL) && inputManager->isKeyDown(KEYBOARD_LALT) && inputManager->isKeyDown(KEYBOARD_BACKSPACE))
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
				if (inputManager->isKeyDown(KEYBOARD_LSHIFT) || inputManager->isKeyDown(KEYBOARD_RSHIFT))
					capital = 32;
				for (unsigned char i = 61; i <= 122; i++)
				{
					if (inputManager->isKeyPressed(i))
					{
						input += i - capital;
						inputReceived = true;
					}
				}

				//Numbers
				if (!inputManager->isKeyDown(KEYBOARD_LSHIFT) && !inputManager->isKeyDown(KEYBOARD_RSHIFT))
				{
					for (unsigned char i = 48; i <= 57; i++)
					{
						if (inputManager->isKeyPressed(i))
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
						if (inputManager->isKeyPressed(i))
						{
							input += i - 10;
							inputReceived = true;
						}
					}
				}

				//Minus
				if (inputManager->isKeyPressed(45))
				{
					input += 45;
					inputReceived = true;
				}
				//Period
				if (inputManager->isKeyPressed(46))
				{
					input += 46;
					inputReceived = true;
				}
				//Space
				if (inputManager->isKeyPressed(32))
				{
					input += 32;
					inputReceived = true;
				}
				//Backspace
				if (inputManager->isKeyDown(8))
				{
					if (backspaceTimer <= time::Time::zero && input.size() > 0)
					{
						if (inputManager->isKeyDown(KEYBOARD_LCTRL) || inputManager->isKeyDown(KEYBOARD_RCTRL))
						{//Erase a word
							if (input.back() == ' ')
								input.pop_back();
							else
							{//Erase until space or empty
								while (input.size() > 0 && input.back() != ' ')
									input.pop_back();
								if (input.size() == 1 && input.back() == ' ')
									input.pop_back();
								backspaceTimer = time::seconds(BACKSPACE_INITIAL_INTERVAL);
							}
						}
						else
						{//Erase one character
							input.pop_back();
							if (inputManager->isKeyPressed(8))
							{
								backspaceTimer = time::seconds(BACKSPACE_INITIAL_INTERVAL);
							}
							else
							{
								backspaceTimer = time::milliseconds(BACKSPACE_INTERVAL - backspaceAcceleration);
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
				if (inputManager->isKeyPressed(KEYBOARD_RETURN))
				{
					executeConsole();
					close();
					inputReceived = true;
				}

				//Escape
				if (inputManager->isKeyPressed(KEYBOARD_ESCAPE))
				{
					close();
					inputReceived = true;
				}

				//Previous commands
				if (inputManager->isKeyPressed(KEYBOARD_UP) && previousCommands.size() > 0)
				{
					if (previousCommandIndex == -1)
						previousCommandIndex = previousCommands.size() - 1;
					else if (--previousCommandIndex < 0)
						previousCommandIndex = previousCommands.size() - 1;
					input = previousCommands[previousCommandIndex];
					inputReceived = true;
				}
				if (inputManager->isKeyPressed(KEYBOARD_DOWN) && previousCommands.size() > 0)
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
		void render(std::string customDebugText)
		{
			LockGuardRecursive regionLock(consoleMutex);

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
					fpsCounter->setPosition(spehs::vec2(CONSOLE_BORDER, spehs::ApplicationData::getWindowHeight() - fpsCounter->getTextHeight()));
					frameCounter = 0;
				}
			}
			else
				fpsCounter->setRenderState(false);

			drawCalls = 0;
			vertexDrawCount = 0;

			consoleBatchManager->render();

			//Render lines
			for (unsigned i = 0; i < lines.size(); i++)
				lines[i]->setAlpha(visibility * (spehs::ApplicationData::consoleTextAlpha));

			//Console text
			consoleText->setAlpha(visibility * (spehs::ApplicationData::consoleTextAlpha));

			//Console background shade
			if (consoleText->getRenderState() || (lines.size() > 0 && lines.front()->getRenderState()))
			{
				backgroundShade->setRenderState(true);
				backgroundShade->setAlpha(visibility * 0.8f);
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
		void addVariable(std::string str, bool& var)
		{
			LockGuardRecursive regionLock(consoleMutex);
			boolVariables.push_back(ConsoleVariable<bool>(str, var));
		}
		void addVariable(std::string str, float& var)
		{
			LockGuardRecursive regionLock(consoleMutex);
			floatVariables.push_back(ConsoleVariable<float>(str, var));
		}
		void addVariable(std::string str, int& var)
		{
			LockGuardRecursive regionLock(consoleMutex);
			intVariables.push_back(ConsoleVariable<int>(str, var));
		}
		void addVariable(std::string str, std::string& var)
		{
			LockGuardRecursive regionLock(consoleMutex);
			stringVariables.push_back(ConsoleVariable<std::string>(str, var));
		}
		void addCommand(std::string str, void(*fnc)(void))
		{
			LockGuardRecursive regionLock(consoleMutex);
			commands.push_back(ConsoleCommand(str, fnc));
		}
		void addCommand(std::string str, void(*fnc)(std::vector<std::string>&))
		{
			LockGuardRecursive regionLock(consoleMutex);
			commands.push_back(ConsoleCommand(str, fnc));
		}
		bool removeCommand(std::string commandIdentifier)
		{
			LockGuardRecursive regionLock(consoleMutex);
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
		bool removeVariable(std::string identifier)
		{
			LockGuardRecursive regionLock(consoleMutex);
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
		void clearVariables()
		{
			LockGuardRecursive regionLock(consoleMutex);
			boolVariables.clear();
			floatVariables.clear();
			intVariables.clear();
			stringVariables.clear();
		}
		void clearCommands()
		{
			LockGuardRecursive regionLock(consoleMutex);
			commands.clear();
		}


		//Console logging
		void log(const std::string str, const Color color)
		{
			log(&str[0], str.size(), color);
		}
		void log(const char* str, const unsigned length, const Color color)
		{
			//Validate string characters
			std::string string(str, length);
			for (unsigned i = 0; i < length; i++)
			{
				if (string[i] < 32 && string[i] != '\n')
					string[i] = 32;
			}

			LockGuardRecursive regionLock(consoleMutex);
			newLines.push_back(std::make_pair(string, color));
		}
		void clearLog()
		{
			LockGuardRecursive regionLock(consoleMutex);
			while (!lines.empty())
			{
				lines.back()->destroy();
				lines.pop_back();
			}
		}
		void setPlaneDepth(int16_t depth)
		{
			planeDepth = depth;
			consoleText->setPlaneDepth(depth);
			for (unsigned i = 0; i < lines.size(); i++)
				lines[i]->setPlaneDepth(depth);
		}



		//Local functions
		void updateLinePositions()
		{
			LockGuardRecursive regionLock(consoleMutex);
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
		void executeConsole()
		{
			LockGuardRecursive regionLock(consoleMutex);
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
					static const Color color(0.8f, 0.8f, 0.8f);
					log("-------------------", color);
					log("Spehs Engine select memory allocations:", color);
					log("Remaining allocations / Total (runtime) allocations", color);
					log("GUI Rectangles: " + std::to_string(guiRectangleAllocations - guiRectangleDeallocations) + "/" + std::to_string(guiRectangleAllocations), color);
					log("Primitives: " + std::to_string(primitiveAllocations - primitiveDeallocations) + "/" + std::to_string(primitiveAllocations), color);
					log("PrimitiveBatches: " + std::to_string(BatchAllocations - BatchDeallocations) + "/" + std::to_string(BatchAllocations), color);
					log("Serializables: " + std::to_string(serializableAllocations - serializableDeallocations) + "/" + std::to_string(serializableAllocations), color);
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
		void setVariable()
		{
			LockGuardRecursive regionLock(consoleMutex);
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
}