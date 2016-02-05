#include "ApplicationData.h"
#include <fstream>
#include <iostream>
#include <string>
#define APPLICATION_DATA_FILE_DIRECTORY "SaveFiles/ApplicationData.txt"


SpehsEngine::ApplicationData* applicationData;
namespace SpehsEngine
{
	void readValueIntoInt(std::ifstream& stream, int& integer)
	{
		std::string line;
		std::string intString = "";
		std::getline(stream, line);
		for (unsigned i = 0; i < line.size(); i++)
		{
			if (line[i] == ':')
			{
				for (unsigned l = i + 1; l < line.size(); l++)
				{
					if (line[l] != ' ')
						intString += line[l];
				}
				break;
			}
		}
		integer = atoi(intString.c_str());
	}
	ApplicationData::ApplicationData() :
		windowWidth(1280), windowHeight(720), windowMode(0),
		masterVolume(100), musicVolume(100), sfxVolume(100),
		showFps(1), consoleTextSize(20), consoleTextAlpha(1000), vSync(0), MSAA(4)
	{

	}
	ApplicationData::~ApplicationData()
	{

	}
	void ApplicationData::save()
	{
		std::ofstream stream;
		stream.open(APPLICATION_DATA_FILE_DIRECTORY, std::ios::trunc);
		if (stream.fail())
		{
			std::cout << "\nFailed to save application data!";
			return;
		}

		stream << "Window width: " << windowWidth << "\n";
		stream << "Window height: " << windowHeight << "\n";
		stream << "Window mode: " << windowMode << "\n";
		stream << "Master volume: " << masterVolume << "\n";
		stream << "Music volume: " << musicVolume << "\n";
		stream << "SFX volume: " << sfxVolume << "\n";
		stream << "Show FPS: " << showFps << "\n";
		stream << "Console text size: " << consoleTextSize << "\n";
		stream << "ConsoleTextAlpha (divided by 1000): " << consoleTextAlpha << "\n";
		stream << "VSync: " << vSync << "\n";
		stream << "MSAA: " << MSAA << "\n";

		stream.close();
	}
	void ApplicationData::load()
	{
		std::ifstream stream;
		stream.open(APPLICATION_DATA_FILE_DIRECTORY);
		if (stream.fail())
		{
			std::cout << "\nFailed to save application data!";
			return;
		}

		readValueIntoInt(stream, windowWidth);
		readValueIntoInt(stream, windowHeight);
		readValueIntoInt(stream, windowMode);
		readValueIntoInt(stream, masterVolume);
		readValueIntoInt(stream, musicVolume);
		readValueIntoInt(stream, sfxVolume);
		readValueIntoInt(stream, showFps);
		readValueIntoInt(stream, consoleTextSize);
		readValueIntoInt(stream, consoleTextAlpha);
		readValueIntoInt(stream, vSync);
		readValueIntoInt(stream, MSAA);

		stream.close();
	}
}