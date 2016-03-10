#include <iostream>
#include <mutex>
#include "BitwiseOperations.h"
#include "Exceptions.h"
#include "InputManager.h"
#include "ApplicationData.h"
#include "Time.h"
#include "Console.h"
#include "ConsoleVariable.h"
#include "Text.h"
#define CONSOLE_COMMANDS_KEPT_IN_MEMORY 10
#define LOG_LINES_KEPT_IN_MEMORY 25
#define CONSOLE_BORDER 5
#define BACKSPACE_INITIAL_INTERVAL 500
#define BACKSPACE_INTERVAL 75
#define CONSOLE_INITIALIZED_BIT 1
#define CONSOLE_OPEN_BIT 2
#define CONSOLE_LOCK_ENABLED_BIT 4
extern int64_t guiRectangleAllocations;
extern int64_t guiRectangleDeallocations;
extern int64_t primitiveBatchAllocations;
extern int64_t primitiveBatchDeallocations;
typedef std::lock_guard<std::recursive_mutex> lockGuardRecursive;



namespace spehs
{
	namespace console
	{
		//Static console data
		static int previousFontSize = 10;
		static int state = 0;
		static int backspaceTimer = 0;
		static int backspaceAcceleration = 0;
		static int previousCommandIndex = 0;
		static float visibility = 1.0f;
		static Text* consoleText;
		static std::recursive_mutex consoleMutex;
		static std::string input;
		static std::vector<Text*> lines;
		static std::vector<ConsoleCommand> commands;
		static std::vector<std::string> consoleWords;
		static std::vector<ConsoleVariable<int>> intVariables;
		static std::vector<ConsoleVariable<float>> floatVariables;
		static std::vector<ConsoleVariable<bool>> boolVariables;
		static std::vector<ConsoleVariable<std::string>> stringVariables;
		static std::vector<std::string> previousCommands;

		//Local methods
		void updateLinePositions();
		void setVariable();
		void executeConsole();


		//Console state
		int initialize()
		{
			lockGuardRecursive regionLock(consoleMutex);
			if (checkBit(state, CONSOLE_INITIALIZED_BIT))
			{
				log("Console already initialized!");
				return 0;
			}
			consoleText = new spehs::Text();
			consoleText->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
			consoleText->setColor(glm::vec4(1.0f, 0.6f, 0.0f, applicationData->consoleTextAlpha / 255.0f));
			consoleText->setPosition(glm::vec2(CONSOLE_BORDER, CONSOLE_BORDER));
			consoleText->setString("><");

			enableBit(state, CONSOLE_INITIALIZED_BIT);
			log("Console initialized");
			return 0;
		}
		void unitialize()
		{
			lockGuardRecursive regionLock(consoleMutex);
			if (!checkBit(state, CONSOLE_INITIALIZED_BIT))
			{
				std::cout << "\nSpehsEngine::console already uninitialized!";
				return;
			}
			delete consoleText;
			consoleText = nullptr;
			clearLog();
			disableBit(state, CONSOLE_INITIALIZED_BIT);
		}
		void open()
		{
			lockGuardRecursive regionLock(consoleMutex);
			if (checkBit(state, CONSOLE_OPEN_BIT))
				return;
			enableBit(state, CONSOLE_OPEN_BIT);
			updateLinePositions();
		}
		void close()
		{
			lockGuardRecursive regionLock(consoleMutex);
			if (!checkBit(state, CONSOLE_OPEN_BIT))
				return;
			disableBit(state, CONSOLE_OPEN_BIT);
			updateLinePositions();
			input.clear();
		}
		bool isOpen()
		{
			lockGuardRecursive regionLock(consoleMutex);
			return checkBit(state, CONSOLE_OPEN_BIT);
		}


		//Update cycle
		void update()
		{
			lockGuardRecursive regionLock(consoleMutex);
			if (visibility > 0.0f)
			{
				//Update console font size if needed
				if (previousFontSize != applicationData->consoleTextSize)
				{
					consoleText->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
					for (unsigned i = 0; i < lines.size(); i++)
					{
						lines[i]->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
					}
					previousFontSize = applicationData->consoleTextSize;
				}

				if (!isOpen())
				{
					visibility -= deltaTime / 1000.0f;
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
					visibility += deltaTime / 200.0f;
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
								{
									input.pop_back();
								}
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
						backspaceTimer -= deltaTime;
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
		void render()
		{
			lockGuardRecursive regionLock(consoleMutex);

			//Render lines
			if (visibility < 0.01f)
			{
				return;
			}
			for (auto i = lines.begin(); i < lines.end(); i++)
			{
				(*i)->getColorRef().a = visibility * (applicationData->consoleTextAlpha / 255.0f);
				(*i)->render();
			}

			//Render console text
			if (!isOpen())
			{
				return;
			}
			consoleText->getColorRef().a = visibility * (applicationData->consoleTextAlpha / 255.0f);
			consoleText->render();
		}



		//Console variables/commands
		void addVariable(std::string str, bool& var)
		{
			lockGuardRecursive regionLock(consoleMutex);
			boolVariables.push_back(ConsoleVariable<bool>(str, var));
		}
		void addVariable(std::string str, float& var)
		{
			lockGuardRecursive regionLock(consoleMutex);
			floatVariables.push_back(ConsoleVariable<float>(str, var));
		}
		void addVariable(std::string str, int& var)
		{
			lockGuardRecursive regionLock(consoleMutex);
			intVariables.push_back(ConsoleVariable<int>(str, var));
		}
		void addVariable(std::string str, std::string& var)
		{
			lockGuardRecursive regionLock(consoleMutex);
			stringVariables.push_back(ConsoleVariable<std::string>(str, var));
		}
		void addCommand(std::string str, void(*fnc)(std::vector<std::string>&))
		{
			lockGuardRecursive regionLock(consoleMutex);
			commands.push_back(ConsoleCommand(str, fnc));
		}
		void clearVariables()
		{
			lockGuardRecursive regionLock(consoleMutex);
			boolVariables.clear();
			floatVariables.clear();
			intVariables.clear();
			stringVariables.clear();
		}
		void clearCommands()
		{
			lockGuardRecursive regionLock(consoleMutex);
			commands.clear();
		}


		//Console logging
		void log(std::string str, glm::vec3& color)
		{
			lockGuardRecursive regionLock(consoleMutex);

			if (lines.size() >= LOG_LINES_KEPT_IN_MEMORY)
			{
				lines.erase(lines.begin());
			}
			lines.push_back(new spehs::Text());
			lines.back()->setFont(applicationData->GUITextFontPath, applicationData->consoleTextSize);
			lines.back()->setColor(glm::vec4(color, applicationData->consoleTextAlpha / 255.0f));
			lines.back()->setString(str);
			visibility = 1.0f;

			updateLinePositions();
		}
		void warning(std::string str)
		{
			lockGuardRecursive regionLock(consoleMutex);
			log("[Warning] " + str, glm::vec3(1.0f, 0.3f, 0.0f));
			spehs::exceptions::warning(str);
		}
		void error(std::string str)
		{
			lockGuardRecursive regionLock(consoleMutex);
			log("[Error] " + str, glm::vec3(1.0f, 0.0f, 0.0f));
			spehs::exceptions::unexpectedError(str);
		}
		void clearLog()
		{
			lockGuardRecursive regionLock(consoleMutex);
			while (!lines.empty())
			{
				delete lines.back();
				lines.pop_back();
			}
		}




		//Local functions
		void updateLinePositions()
		{
			lockGuardRecursive regionLock(consoleMutex);
			if (lines.size() == 0)
			{
				return;
			}

			int lineFix = 0;
			if (isOpen())
			{
				lineFix++;
			}
			for (unsigned i = 0; i < lines.size(); i++)
			{
				lines[lines.size() - 1 - i]->setPosition(glm::vec2(CONSOLE_BORDER, CONSOLE_BORDER + lines.back()->getFontHeight()*(i + lineFix)));
			}
		}
		void executeConsole()
		{
			lockGuardRecursive regionLock(consoleMutex);
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
					log("\\\\" + boolVariables[i].identifier);
				for (unsigned i = 0; i < intVariables.size(); i++)
					log("\\\\" + intVariables[i].identifier);
				for (unsigned i = 0; i < floatVariables.size(); i++)
					log("\\\\" + floatVariables[i].identifier);
				for (unsigned i = 0; i < stringVariables.size(); i++)
					log("\\\\" + stringVariables[i].identifier);
				log("Available commands <command parameter1 parameter2 parameterN>");
				for (unsigned i = 0; i < commands.size(); i++)
					log("\\\\" + commands[i].command);
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
					log("Primitive batches: " + std::to_string(primitiveBatchAllocations - primitiveBatchDeallocations) + "/" + std::to_string(primitiveBatchAllocations), color);
					log("-------------------", color);
					return;
				}
				return;
			}
			else
			{//Search for command with matching identifier
				for (unsigned i = 0; i < commands.size(); i++)
				{
					if (commands[i].command == consoleWords[0])
					{
						commands[i].function(consoleWords);
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
			bool isFloat = false;
			for (unsigned i = 0; i < consoleWords[2].size(); i++)
			if ((consoleWords[2][i] < 48 || consoleWords[2][i] > 57) && consoleWords[2][i] != 45)//if the character is not "numeric" (number or -)
			{//Value is not numeric

				//Check booleans
				if (consoleWords[2] == "true" || consoleWords[2] == "false")
				{
					for (unsigned i = 0; i < boolVariables.size(); i++)
					if (consoleWords[1] == boolVariables[i].identifier)
					{
						if (consoleWords[2] == "true")
						{
							log("Setting " + boolVariables[i].identifier + " as true");
							boolVariables[i].set(true);
						}
						else
						{
							log("Setting " + boolVariables[i].identifier + " as false");
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
					if (stringVariables[i].identifier == consoleWords[1])
					{
						log("Setting " + stringVariables[i].identifier + " to " + consoleWords[2]);
						*stringVariables[i].variablePtr = consoleWords[2];
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
				if (floatVariables[i].identifier == consoleWords[1])
				{
					{
						log("Setting " + floatVariables[i].identifier + " to " + consoleWords[2]);
						floatVariables[i].set(atoi(consoleWords[2].c_str()));
						return;
					}
				}
			}
			else
			{
				for (unsigned i = 0; i < intVariables.size(); i++)
				if (intVariables[i].identifier == consoleWords[1])
				{
					{
						log("Setting " + intVariables[i].identifier + " to " + consoleWords[2]);
						intVariables[i].set(atoi(consoleWords[2].c_str()));
						return;
					}
				}
			}
		}
	}
}