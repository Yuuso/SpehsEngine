#include <algorithm>
#include <string>
#include "ApplicationData.h"
#include "FileStream.h"
#include "Console.h"

spehs::ApplicationData* applicationData;
namespace spehs
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
	void readValueIntoString(std::ifstream& stream, std::string& string)
	{
		std::string line;
		std::string value = "";
		std::getline(stream, line);
		for (unsigned i = 0; i < line.size(); i++)
		{
			if (line[i] == ':')
			{
				for (unsigned l = i + 1; l < line.size(); l++)
					value += line[l];
				break;
			}
		}
		if (value[0] == ' ')
			value.erase(value.begin());
		string = value;
	}

	ApplicationData::ApplicationData() :
		//Video
		windowMode(0), showFps(1), maxFps(200), vSync(0), MSAA(4),
		//Audio
		masterVolume(100), musicVolume(100), sfxVolume(100),
		//Other
		consoleTextSize(12), consoleTextAlpha(900),
		GUITextSize(12), GUITextFontPath("Fonts/Anonymous.ttf"),
		dataDirectory("data/"), screenshotDirectory("screenshots/"),
		windowWidth(1280), windowHeight(720)
	{

	}
	ApplicationData::~ApplicationData()
	{

	}
	void ApplicationData::write(std::ofstream* stream)
	{
		bool streamOwner = false;
		if (!stream)
		{//Create file stream
			stream = new std::ofstream("ApplicationData.txt");
			streamOwner = true;
			//stream->open("ApplicationData.txt", std::ios::trunc);
			if (stream->fail())
			{
				spehs::console::error("Failed to write application data!");
				return;
			}
		}

		//Video
		*stream << "Window width: " << windowWidth << "\n";
		*stream << "Window height: " << windowHeight << "\n";
		*stream << "Window mode: " << windowMode << "\n";
		*stream << "Show FPS: " << showFps << "\n";
		*stream << "Max FPS: " << maxFps << "\n";
		*stream << "VSync: " << vSync << "\n";
		*stream << "MSAA: " << MSAA << "\n";
		//Audio
		*stream << "Master volume: " << int(100 * masterVolume) << "\n";
		*stream << "Music volume: " << int(100 * musicVolume) << "\n";
		*stream << "SFX volume: " << int(100 * sfxVolume) << "\n";
		//Other
		*stream << "Console text size: " << consoleTextSize << "\n";
		*stream << "ConsoleTextAlpha (0-255): " << consoleTextAlpha << "\n";
		*stream << "GUI text size: " << GUITextSize << "\n";
		*stream << "GUI text font path: " << GUITextFontPath << "\n";
		*stream << "Data directory: " << dataDirectory << "\n";
		*stream << "Screenshot directory: " << screenshotDirectory << "\n";

		if (streamOwner)
		{
			stream->close();
			delete stream;		
		}
	}
	void ApplicationData::read(std::ifstream* stream)
	{
		bool streamOwner = false;
		if (!stream)
		{//Create file stream
			stream = new std::ifstream;
			streamOwner = true;
			stream->open("ApplicationData.txt");
			if (stream->fail())
			{//Application data does not exist
				std::ofstream ofstream;
				ofstream.open("ApplicationData.txt", std::ios::trunc);
				if (ofstream.fail())
				{
					spehs::console::error("Failed to write application data!");
					delete stream;
					return;
				}
				write(&ofstream);
				ofstream.close();
				stream->open("ApplicationData.txt");
				if (stream->fail())
				{
					spehs::console::error("Failed to create (SpehsData)application data");
					delete stream;
					return;
				}
			}
		}

		//Video
		readValueIntoInt(*stream, windowWidth);
		readValueIntoInt(*stream, windowHeight);
		readValueIntoInt(*stream, windowMode);
		readValueIntoInt(*stream, showFps);
		readValueIntoInt(*stream, maxFps);
		readValueIntoInt(*stream, vSync);
		readValueIntoInt(*stream, MSAA);
		//Audio
		int intVal;
		readValueIntoInt(*stream, intVal);		masterVolume = std::min(1.0f, std::max(0.0f, intVal * 0.01f));
		readValueIntoInt(*stream, intVal);		musicVolume = std::min(1.0f, std::max(0.0f, intVal * 0.01f));
		readValueIntoInt(*stream, intVal);		sfxVolume = std::min(1.0f, std::max(0.0f, intVal * 0.01f));
		//Other
		readValueIntoInt(*stream, consoleTextSize);
		readValueIntoInt(*stream, consoleTextAlpha);
		readValueIntoInt(*stream, GUITextSize);
		readValueIntoString(*stream, GUITextFontPath);
		readValueIntoString(*stream, dataDirectory);
		readValueIntoString(*stream, screenshotDirectory);

		//Limit values
		if (windowWidth < 1)
			windowWidth = 1;
		if (windowHeight < 1)
			windowHeight = 1;
		if (masterVolume < 0)
			masterVolume = 0;
		else if (masterVolume > 100)
			masterVolume = 100;
		if (musicVolume < 0)
			musicVolume = 0;
		else if (musicVolume > 100)
			musicVolume = 100;
		if (sfxVolume < 0)
			sfxVolume = 0;
		else if (sfxVolume > 100)
			sfxVolume = 100;
		if (consoleTextAlpha < 0)
			consoleTextAlpha = 0;
		else if (consoleTextAlpha > 255)
			consoleTextAlpha = 255;
		if (consoleTextSize < 1)
			consoleTextSize = 1;
		if (GUITextSize < 1)
			GUITextSize = 1;
		
		//Verify directories
		if (dataDirectory.size() > 0)
			verifyDirectory(dataDirectory);
		if (screenshotDirectory.size() > 0)
			verifyDirectory(screenshotDirectory);

		if (streamOwner)
		{
			stream->close();
			delete stream;
		}
	}
}