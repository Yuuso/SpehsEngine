#include <iostream>
#include "InputManager.h"
#include "ApplicationData.h"
#include "Time.h"
#include "Console.h"
#define CONSOLE_FONT_PATH "Fonts/Anonymous.ttf"
#define CONSOLE_BORDER 5
#define BACKSPACE_INITIAL_INTERVAL 500
#define BACKSPACE_INTERVAL 75
static int previousFontSize = 10;
static  bool colorUpdated = true;

SpehsEngine::Console* console;

namespace SpehsEngine
{
	Console::Console(){}
	Console::~Console(){}
	int Console::initialize()
	{
		consoleText = new SpehsEngine::Text();
		consoleText->setFont(CONSOLE_FONT_PATH, applicationData->consoleTextSize);
		consoleText->setColor(glm::vec4(1.0f, 0.6f, 0.0f, applicationData->consoleTextAlpha / 1000.0f));
		consoleText->setPosition(glm::vec2(CONSOLE_BORDER, CONSOLE_BORDER));
		consoleText->setString("><");


		//Add console variables
		addVariable("fontSize", applicationData->consoleTextSize);


		log("Console initialized");
		return 0;
	}
	void Console::unitialize()
	{
		delete consoleText;
		consoleText = nullptr;
		clearLog();
	}

	void Console::addVariable(std::string str, bool& var)
	{
		boolVariables.push_back(ConsoleVariable<bool>(str, var));
	}
	void Console::addVariable(std::string str, float& var)
	{
		floatVariables.push_back(ConsoleVariable<float>(str, var));
	}
	void Console::addVariable(std::string str, int& var)
	{
		intVariables.push_back(ConsoleVariable<int>(str, var));
	}
	void Console::addConsoleCommand(std::string str, void(*fnc)(std::vector<std::string>&))
	{
		commands.push_back(ConsoleCommand(str, fnc));
	}

	void Console::log(std::string str)
	{
		lines.push_back(new SpehsEngine::Text());
		lines.back()->setFont(CONSOLE_FONT_PATH, applicationData->consoleTextSize);
		lines.back()->setColor(glm::vec4(1.0f, 0.6f, 0.0f, applicationData->consoleTextAlpha / 1000.0f));
		lines.back()->setString(str);
		visibility = 1.0f;

		updateLinePositions();
	}
	void Console::update()
	{
		if (visibility > 0.0f)
		{
			//Update console font size if needed
			if (previousFontSize != applicationData->consoleTextSize)
			{
				consoleText->setFont(CONSOLE_FONT_PATH, applicationData->consoleTextSize);
				for (unsigned i = 0; i < lines.size(); i++)
				{
					lines[i]->setFont(CONSOLE_FONT_PATH, applicationData->consoleTextSize);
				}
				previousFontSize = applicationData->consoleTextSize;
			}

			if (!open)
			{
				visibility -= deltaTime / 1000.0f;
			}
		}

		if (!open)
		{
			//Console is not open, check opening key combination
			if (inputManager->isKeyDown(SDLK_LCTRL) && inputManager->isKeyDown(SDLK_LALT) && inputManager->isKeyDown(SDLK_BACKSPACE))
			{
				openConsole();
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
			if (inputManager->isKeyDown(SDLK_LSHIFT) || inputManager->isKeyDown(SDLK_RSHIFT))
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
			for (unsigned char i = 48; i <= 57; i++)
			{
				if (inputManager->isKeyPressed(i))
				{
					input += i;
					inputReceived = true;
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
			if (inputManager->isKeyDown(8) || inputManager->isKeyPressed(8))
			{
				if (backspaceTimer <= 0)
				{//Erase one character
					if (input.size() > 0)
					{
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
						inputReceived = true;
					}
				}
				else
				{
					//backspaceTimer -= deltaTime; //TODO
				}
			}
			else
			{
				backspaceTimer = 0;
				backspaceAcceleration = 0;
			}

			//Enter
			if (inputManager->isKeyPressed(SDLK_RETURN))
			{
				executeConsole();
				closeConsole();
			}


			if (inputReceived)
			{
				consoleText->setString('>' + input + '<');
			}
		}
	}
	void Console::updateLinePositions()
	{
		if (lines.size() == 0)
		{
			return;
		}

		int lineFix = 0;
		if (open)
		{
			lineFix++;
		}
		for (unsigned i = 0; i < lines.size(); i++)
		{
			lines[lines.size() - 1 - i]->setPosition(glm::vec2(CONSOLE_BORDER, CONSOLE_BORDER + lines.back()->getFontHeight()*(i + lineFix)));
		}
	}


	void Console::render()
	{
		//Render lines
		if (visibility < 0.01f)
		{
			return;
		}
		for (auto i = lines.begin(); i < lines.end(); i++)
		{
			(*i)->getColorRef().a = visibility * (applicationData->consoleTextAlpha / 1000.0f);
			(*i)->render();
		}

		//Render console text
		if (!open)
		{
			return;
		}
		consoleText->getColorRef().a = visibility * (applicationData->consoleTextAlpha / 1000.0f);
		consoleText->render();
	}

	void Console::executeConsole()
	{
		if (input.size() == 0)
			return;

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

		//Set variables
		if (consoleWords[0] == "set")
		{
			if (consoleWords.size() >= 3)
			{
				setVariable();
				foundCommand = true;
			}
			else
			{
				log("Set failed! Too few parameters in command call");
			}
		}
		else
		{
			//Search for command with matching identifier
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
			log("Unkown command");
		}
	}
	void Console::setVariable()
	{
		bool foundVariable = false;
		bool isFloat = false;
		for (unsigned i = 0; i < consoleWords[2].size(); i++)
			if ((consoleWords[2][i] < 48 || consoleWords[2][i] > 57) && consoleWords[2][i] != 45)//if the character is not "numeric" (number or -)
			{//Value is not numeric
				if (consoleWords[2] == "true" || consoleWords[2] == "false")
				{
					for (unsigned i = 0; i < boolVariables.size(); i++)
						if (consoleWords[1] == boolVariables[i].identifier)
						{
							if (consoleWords[2] == "true")
							{
								foundVariable = true;
								boolVariables[i].set(true);
								log("Setting " + boolVariables[i].identifier + " as true");
							}
							else
							{
								foundVariable = true;
								boolVariables[i].set(false);
								log("Setting " + boolVariables[i].identifier + " as false");
							}
							return;
						}
					log("Invalid command!");
					return;
				}
			}
			else if (consoleWords[2][i] == 46)//Test for period (decimal numbers/aka floats)
			{
				if (isFloat == false)
					isFloat = true;
				else
				{
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
						foundVariable = true;
						floatVariables[i].set(atoi(consoleWords[2].c_str()));
						log("Setting " + floatVariables[i].identifier + " to " + consoleWords[2]);
					}
				}
		}
		else
		{
			for (unsigned i = 0; i < intVariables.size(); i++)
				if (intVariables[i].identifier == consoleWords[1])
				{
					{
						foundVariable = true;
						intVariables[i].set(atoi(consoleWords[2].c_str()));
						log("Setting " + intVariables[i].identifier + " to " + consoleWords[2]);
					}
				}
		}
		if (!foundVariable)
		{
			log("Unkown identifier [" + consoleWords[1] + "]!");
		}
	}
	void Console::clearLog()
	{
		while (!lines.empty())
		{
			delete lines.back();
			lines.pop_back();
		}
	}
	void Console::openConsole()
	{
		open = true;
		updateLinePositions();
	}
	void Console::closeConsole()
	{
		open = false;
		updateLinePositions();
	}
}