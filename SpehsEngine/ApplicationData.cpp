#include "ApplicationData.h"
#include "FileStream.h"
#include "Console.h"
#include <string>


SpehsEngine::ApplicationData* applicationData;
namespace SpehsEngine
{
	void ApplicationData::readValueIntoInt(std::ifstream& stream, int& integer)
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
	void ApplicationData::readValueIntoString(std::ifstream& stream, std::string& string)
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
		windowWidth(1280), windowHeight(720), windowMode(0),
		masterVolume(100), musicVolume(100), sfxVolume(100),
		showFps(1), consoleTextSize(20), consoleTextAlpha(1000), vSync(0), MSAA(4), dataDirectory("data/")
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
			stream = new std::ofstream;
			streamOwner = true;
			stream->open("ApplicationData.txt", std::ios::trunc);
			if (stream->fail())
			{
				console->error("Failed to write application data!");
				return;
			}
		}

		*stream << "Window width: " << windowWidth << "\n";
		*stream << "Window height: " << windowHeight << "\n";
		*stream << "Window mode: " << windowMode << "\n";
		*stream << "Master volume: " << masterVolume << "\n";
		*stream << "Music volume: " << musicVolume << "\n";
		*stream << "SFX volume: " << sfxVolume << "\n";
		*stream << "Show FPS: " << showFps << "\n";
		*stream << "Console text size: " << consoleTextSize << "\n";
		*stream << "ConsoleTextAlpha (divided by 1000): " << consoleTextAlpha << "\n";
		*stream << "VSync: " << vSync << "\n";
		*stream << "MSAA: " << MSAA << "\n";
		*stream << "Data directory: " << dataDirectory << "\n";

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
					console->error("Failed to write application data!");
					delete stream;
					return;
				}
				write(&ofstream);
				ofstream.close();
				stream->open("ApplicationData.txt");
				if (stream->fail())
				{
					console->error("Failed to create (SpehsData)application data");
					delete stream;
					return;
				}
			}
		}

		readValueIntoInt(*stream, windowWidth);
		readValueIntoInt(*stream, windowHeight);
		readValueIntoInt(*stream, windowMode);
		readValueIntoInt(*stream, masterVolume);
		readValueIntoInt(*stream, musicVolume);
		readValueIntoInt(*stream, sfxVolume);
		readValueIntoInt(*stream, showFps);
		readValueIntoInt(*stream, consoleTextSize);
		readValueIntoInt(*stream, consoleTextAlpha);
		readValueIntoInt(*stream, vSync);
		readValueIntoInt(*stream, MSAA);
		readValueIntoString(*stream, dataDirectory);
		
		//Create data directory
		createDirectory(dataDirectory);

		if (streamOwner)
		{
			stream->close();
			delete stream;
		}
	}
}