
#include <iostream>
#include <mutex>

#include "BitwiseOperations.h"
#include "Exceptions.h"
#include "InputManager.h"
#include "ApplicationData.h"
#include "Time.h"
#include "Console.h"
#include "Camera2D.h"
#include "ConsoleVariable.h"
#include "Text.h"
#include "BatchManager.h"

//State
#define CONSOLE_INITIALIZED_BIT				0x0001
#define CONSOLE_OPEN_BIT					0x0002
#define CONSOLE_TEXT_EXECUTED_BIT			0x0004
//Misc
#define CONSOLE_COMMANDS_KEPT_IN_MEMORY 10
#define LOG_LINES_KEPT_IN_MEMORY 25
#define CONSOLE_BORDER 5
#define BACKSPACE_INITIAL_INTERVAL 500
#define BACKSPACE_INTERVAL 75
#define FADE_OUT_TIME 2.5f
//FPS
#define FPS_REFRESH_RATE 5


extern int64_t guiRectangleAllocations;
extern int64_t guiRectangleDeallocations;
extern int64_t primitiveAllocations;
extern int64_t primitiveDeallocations;
extern int64_t BatchAllocations;
extern int64_t BatchDeallocations;
extern int64_t textAllocations;
extern int64_t textDeallocations;
extern int64_t collisionPointAllocations;
extern int64_t collisionPointDeallocations;
typedef std::lock_guard<std::recursive_mutex> LockGuardRecursive;



namespace spehs
{
	namespace console
	{
		//Static console data
		static int16_t state = 0;
		static uint16_t planeDepth = 10000;
		static int backspaceTimer = 0;
		static int backspaceAcceleration = 0;
		static int previousCommandIndex = 0;
		static int previousFontSize = 10;
		static float visibility = 1.0f;
		unsigned long drawCalls;
		unsigned long vertexDrawCount;
		static Text* consoleText;
		spehs::Text* fpsCounter;
		static std::recursive_mutex consoleMutex;
		static std::string input;
		static std::string textExecuted;///< Text executed without '/'
		static std::vector<Text*> lines;
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
			
			fpsCounter = consoleBatchManager->createText(10000);
			if (!fpsCounter)
			{
				std::cout << "\nInitialization failed! Failed to create fpsCounter!";
				return false;
			}
			fpsCounter->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);

			fpsCounter->setColor(glm::vec4(1.0f, 0.3f, 0.0f, 0.85f));
			fpsCounter->setString("FPS:0123456789\nDraw calls:0123456789\nVertices:0123456789");
			fpsCounter->setPosition(glm::vec2(5, applicationData->getWindowHeight() - fpsCounter->getTextHeight()));

			consoleText = consoleBatchManager->createText();
			consoleText->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
			consoleText->setColor(glm::vec4(1.0f, 0.6f, 0.0f, applicationData->consoleTextAlpha / 255.0f));
			consoleText->setPosition(glm::vec2(CONSOLE_BORDER, CONSOLE_BORDER));
			consoleText->setString("><");
			consoleText->setRenderState(checkState(CONSOLE_OPEN_BIT));
			setPlaneDepth(planeDepth);

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

			if (fpsCounter != nullptr)
			{
				fpsCounter->destroy();
				fpsCounter = nullptr;
			}

			delete consoleBatchManager;
			delete consoleCamera;

			if (consoleText)
				consoleText->destroy();
			consoleText = nullptr;
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
		bool isOpen()
		{
			LockGuardRecursive regionLock(consoleMutex);
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
		void update()
		{
			LockGuardRecursive regionLock(consoleMutex);
			disableState(CONSOLE_TEXT_EXECUTED_BIT);
			if (visibility > 0.0f)
			{
				//Update console font size if needed
				if (previousFontSize != applicationData->consoleTextSize)
				{
					fpsCounter->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
					consoleText->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
					for (unsigned i = 0; i < lines.size(); i++)
						lines[i]->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
					previousFontSize = applicationData->consoleTextSize;

				}

				if (!isOpen())
				{
					visibility -= time::getDeltaTimeAsSeconds() / FADE_OUT_TIME;
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
					visibility += time::getDeltaTimeAsSeconds() * 5.0f;
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
					if (backspaceTimer <= 0 && input.size() > 0)
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
								backspaceTimer = BACKSPACE_INITIAL_INTERVAL;
							}
						}
						else
						{//Erase one character
							input.pop_back();
							if (inputManager->isKeyPressed(8))
							{
								backspaceTimer = BACKSPACE_INITIAL_INTERVAL;
							}
							else
							{
								backspaceTimer = BACKSPACE_INTERVAL - backspaceAcceleration;
								backspaceAcceleration += 3;
							}
						}
						inputReceived = true;
					}
					else
					{
						backspaceTimer -= time::getDeltaTimeAsMilliseconds();
					}
				}
				else
				{
					backspaceTimer = 0;
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

			if (applicationData->showFps)
			{
				fpsCounter->setRenderState(true);

				static int frameCounter = 0;
				if (++frameCounter >= FPS_REFRESH_RATE)
				{
					fpsCounter->setString("FPS: " + std::to_string(int(time::getFPS())) + "\nDraw calls: " + std::to_string(drawCalls) + "\nVertices: " + std::to_string(vertexDrawCount) + "\n" + customDebugText);
					fpsCounter->setPosition(glm::vec2(5, applicationData->getWindowHeight() - fpsCounter->getTextHeight()));
					frameCounter = 0;
				}
			}
			else
			{
				fpsCounter->setRenderState(false);
			}

			drawCalls = 0;
			vertexDrawCount = 0;

			consoleBatchManager->render();

			//Render lines
			for (unsigned i = 0; i < lines.size(); i++)
				lines[i]->setAlpha(visibility * (applicationData->consoleTextAlpha / 255.0f));

			//Console text
			consoleText->setAlpha(visibility * (applicationData->consoleTextAlpha / 255.0f));
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
		void log(std::string str, glm::vec3& color)
		{
			log(&str[0], str.size(), color);
		}
		void log(char* str, unsigned length, glm::vec3& color)
		{
			LockGuardRecursive regionLock(consoleMutex);

			if (lines.size() >= LOG_LINES_KEPT_IN_MEMORY)
			{
				lines.front()->destroy();
				lines.erase(lines.begin());
			}
			lines.push_back(consoleBatchManager->createText(planeDepth));
			lines.back()->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
			lines.back()->setColor(glm::vec4(color, applicationData->consoleTextAlpha / 255.0f));
			lines.back()->setString(str);
			visibility = 1.0f;
			for (unsigned i = 0; i < lines.size(); i++)
				lines[i]->setRenderState(true);

			updateLinePositions();
		}
		void warning(std::string str)
		{
			LockGuardRecursive regionLock(consoleMutex);
			log("[Warning] " + str, glm::vec3(1.0f, 0.3f, 0.0f));
			spehs::exceptions::warning(str);
		}
		void error(std::string str)
		{
			LockGuardRecursive regionLock(consoleMutex);
			log("[Error] " + str, glm::vec3(1.0f, 0.0f, 0.0f));
			spehs::exceptions::unexpectedError(str);
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
				lines[i]->setPosition(glm::vec2(CONSOLE_BORDER, CONSOLE_BORDER + y));
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
					glm::vec3 color(0.8f, 0.8f, 0.8f);
					log("-------------------", color);
					log("Spehs Engine select memory allocations:", color * 0.75f);
					log("Remaining allocations / Total (runtime) allocations", color);
					log("GUI Rectangles: " + std::to_string(guiRectangleAllocations - guiRectangleDeallocations) + "/" + std::to_string(guiRectangleAllocations), color);
					log("Primitives: " + std::to_string(primitiveAllocations - primitiveDeallocations) + "/" + std::to_string(primitiveAllocations), color);
					log("PrimitiveBatches: " + std::to_string(BatchAllocations - BatchDeallocations) + "/" + std::to_string(BatchAllocations), color);
					log("CollisionPoints: " + std::to_string(collisionPointAllocations - collisionPointDeallocations) + "/" + std::to_string(collisionPointAllocations), color);
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