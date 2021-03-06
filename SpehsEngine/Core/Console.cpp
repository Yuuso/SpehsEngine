#include "stdafx.h"

#include <algorithm>
#include <iostream>
#include <atomic>
#include <mutex>

#include <boost/bind.hpp>

#include "SpehsEngine/Core/Console.h"
#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Log.h"

//Misc
#define CONSOLE_COMMANDS_KEPT_IN_MEMORY 10
#define LOG_LINES_KEPT_IN_MEMORY 25
#define CONSOLE_BORDER 5
#define BACKSPACE_INITIAL_INTERVAL 0.5f
#define BACKSPACE_INTERVAL 75
#define FADE_OUT_TIME 2.5f
//FPS
#define FPS_REFRESH_RATE 5


//extern std::atomic<int> guiRectangleAllocations;
//extern std::atomic<int> guiRectangleDeallocations;
//extern std::atomic<int> primitiveAllocations;
//extern std::atomic<int> primitiveDeallocations;
//extern std::atomic<int> BatchAllocations;
//extern std::atomic<int> BatchDeallocations;
//extern std::atomic<int> textAllocations;
//extern std::atomic<int> textDeallocations;



namespace se
{
	const Color Console::defaultLogColor = Color(0.0f, 0.9f, 0.0f, 1.0f);

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

	void Console::addCommand(const std::string& str, const std::function<void(void)>& callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		commands.push_back(ConsoleCommand(str, callback));
	}

	void Console::addCommand(const std::string& str, const std::function<void(const std::vector<std::string>&)>& callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		commands.push_back(ConsoleCommand(str, callback));
	}

	bool Console::removeCommand(const std::string& commandIdentifier)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		for (unsigned i = 0; i < commands.size(); i++)
		{
			if (commands[i].identifier == commandIdentifier)
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
		if (words[0].front() != '/')
			return;
		words[0].erase(words[0].begin());

		if (words[0] == "set")
		{//Set variables
			if (words.size() >= 3)
			{
				bool isFloat = false;
				for (size_t w = 0; w < words[2].size(); w++)
					if ((words[2][w] < 48 || words[2][w] > 57) && words[2][w] != 45)//if the character is not "numeric" (number or -)
					{//Value is not numeric

						//Check booleans
						if (words[2] == "true" || words[2] == "false")
						{
							for (size_t v = 0; v < boolVariables.size(); v++)
								if (words[1] == boolVariables[v]._identifier)
								{
									if (words[2] == "true")
									{
										log("Setting " + boolVariables[v]._identifier + " as true");
										boolVariables[v].set(true);
									}
									else
									{
										log("Setting " + boolVariables[v]._identifier + " as false");
										boolVariables[v].set(false);
									}
									return;
								}
							log("Invalid command!");
							return;
						}

						//Check string variables
						for (unsigned v = 0; v < stringVariables.size(); v++)
						{
							if (stringVariables[v]._identifier == words[1])
							{
								log("Setting " + stringVariables[v]._identifier + " to " + words[2]);
								*stringVariables[v]._variablePtr = words[2];
								return;
							}
						}
						log("Invalid command!");
						return;
					}
					else if (words[2][w] == 46)//Test for period (decimal numbers/aka floats)
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
								floatVariables[i].set((float)atoi(words[2].c_str()));
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
		{
			//Help
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
				log("\\\\" + commands[i].identifier);
			return;
		}
		else if (words[0] == "engine")
		{
			if (words.size() > 1 && words[1] == "memory")
			{
				const Color overrideColor(0.8f, 0.8f, 0.8f);
				log("-------------------", overrideColor);
				log("Spehs Engine select memory allocations:", overrideColor);
				log("Remaining allocations / Total (runtime) allocations", overrideColor);
				//log("GUI Rectangles: " + std::to_string(guiRectangleAllocations - guiRectangleDeallocations) + "/" + std::to_string(guiRectangleAllocations), overrideColor);
				//log("Primitives: " + std::to_string(primitiveAllocations - primitiveDeallocations) + "/" + std::to_string(primitiveAllocations), overrideColor);
				//log("PrimitiveBatches: " + std::to_string(BatchAllocations - BatchDeallocations) + "/" + std::to_string(BatchAllocations), overrideColor);
				log("-------------------", overrideColor);
				return;
			}
		}
		else
		{
			//Search for command with matching identifier
			for (unsigned i = 0; i < commands.size(); i++)
			{
				if (commands[i].identifier == words[0])
				{
					if (commands[i].callback1)
					{
						commands[i].callback1();
					}
					else if (commands[i].callback2)
					{
						commands[i].callback2(words);
					}
					else
					{
						log::error("No callback set");
					}
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
		se_assert(index < lines.size());
		return lines[index];
	}

	const size_t Console::getLineCount() const
	{
		return lines.size();
	}
}