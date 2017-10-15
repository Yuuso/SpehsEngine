#include <algorithm>
#include <string>
#include "ApplicationData.h"
#include "AudioEngine.h"
#include "FileStream.h"
#include "Exceptions.h"
#include "Console.h"

namespace spehs
{
	//DATA
	//public
	int ApplicationData::windowMode = 0;
	int ApplicationData::showFps = 1;
	int ApplicationData::maxFps = 200;
	int ApplicationData::vSync = 0;
	int ApplicationData::MSAA = 4;
	int ApplicationData::consoleTextSize = 12;
	int ApplicationData::consoleTextAlpha = 0.9f;
	int ApplicationData::GUITextSize = 12;
	std::string ApplicationData::GUITextFontPath = "Fonts/Anonymous.ttf";
	std::string ApplicationData::dataDirectory = "data/";
	std::string ApplicationData::screenshotDirectory = "screenshots/";
	//local
	int windowWidth = 1280;
	int windowHeight = 720;
	int windowWidthHalf = windowWidth / 2;
	int windowHeightHalf = windowHeight / 2;
	float masterVolume = 1.0;
	std::function<void(std::ofstream&)>* writeCallback = nullptr;
	std::function<void(std::ifstream&)>* readCallback = nullptr;

	void readValueIntoInt(std::ifstream& stream, int& integer)
	{
		std::string line;
		std::string intString = "";
		std::getline(stream, line);
		for (unsigned i = 0; i < line.size(); i++)
		{
			if (line[i] == '=')
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
			if (line[i] == '=')
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

	bool ApplicationData::write()
	{
		std::ofstream stream(APPLICATION_DATA_FILENAME);
		if (stream.fail())
		{
			spehs::exceptions::unexpectedError("Failed to write application data!");
			return false;
		}

		//Video
		stream << "[Engine]\n";
		stream << "Window width=" << windowWidth << "\n";
		stream << "Window height=" << windowHeight << "\n";
		stream << "Window mode=" << windowMode << "\n";
		stream << "Show FPS=" << showFps << "\n";
		stream << "Max FPS=" << maxFps << "\n";
		stream << "VSync=" << vSync << "\n";
		stream << "MSAA=" << MSAA << "\n";
		//Audio
		stream << "Master volume=" << int(100 * masterVolume) << "\n";
		//Other
		stream << "Console text size=" << consoleTextSize << "\n";
		stream << "ConsoleTextAlpha (0-255)=" << consoleTextAlpha << "\n";
		stream << "GUI text size=" << GUITextSize << "\n";
		stream << "GUI text font path=" << GUITextFontPath << "\n";
		stream << "Data directory=" << dataDirectory << "\n";
		stream << "Screenshot directory=" << screenshotDirectory << "\n";

		stream << "[Application]\n";
		if (writeCallback)
			(*writeCallback)(stream);

		stream.close();
		return true;
	}
	bool ApplicationData::read()
	{
		//Create file stream
		std::ifstream stream(APPLICATION_DATA_FILENAME);
		if (stream.fail())
		{//Application data does not exist
			if (!write())
			{
				spehs::exceptions::unexpectedError("Could not write application data! Failed to read application data!");
				return false;
			}
			stream.open(APPLICATION_DATA_FILENAME);
			if (stream.fail())
			{
				spehs::exceptions::unexpectedError("Failed to read Application data!");
				return false;
			}
		}

		std::string line;
		std::getline(stream, line);//Read section header [Engine]

		//Video
		readValueIntoInt(stream, windowWidth);				windowWidth = std::max(1, windowWidth);
		readValueIntoInt(stream, windowHeight);				windowHeight = std::max(1, windowHeight);
		readValueIntoInt(stream, windowMode);
		readValueIntoInt(stream, showFps);
		readValueIntoInt(stream, maxFps);
		readValueIntoInt(stream, vSync);
		readValueIntoInt(stream, MSAA);
		//Audio
		int intVal;
		readValueIntoInt(stream, intVal);					masterVolume = std::min(1.0f, std::max(0.0f, intVal * 0.01f));
		//Other
		readValueIntoInt(stream, consoleTextSize);			consoleTextSize = std::max(11, consoleTextSize);
		readValueIntoInt(stream, intVal);					consoleTextAlpha = std::min(255, std::max(0, intVal));
		readValueIntoInt(stream, GUITextSize);				GUITextSize = std::max(11, GUITextSize);
		readValueIntoString(stream, GUITextFontPath);
		readValueIntoString(stream, dataDirectory);
		readValueIntoString(stream, screenshotDirectory);
				
		//Verify directories
		if (dataDirectory.size() > 0)
			verifyDirectory(dataDirectory);
		if (screenshotDirectory.size() > 0)
			verifyDirectory(screenshotDirectory);

		//Application defined initialization data
		std::getline(stream, line);//Read section header [Application]
		if (readCallback)
			(*readCallback)(stream);

		stream.close();
	}
	void ApplicationData::setMasterVolume(const float amount)
	{
		masterVolume = amount;
		audio::AudioEngine::updateGain();
	}

	void ApplicationData::setWriteCallback(const std::function<void(std::ofstream&)> callbackFunction)
	{
		if (writeCallback)
			*writeCallback = callbackFunction;
		else
			writeCallback = new std::function<void(std::ofstream&)>(callbackFunction);
	}
	void ApplicationData::setReadCallback(const std::function<void(std::ifstream&)> callbackFunction)
	{
		if (readCallback)
			*readCallback = callbackFunction;
		else
			readCallback = new std::function<void(std::ifstream&)>(callbackFunction);
	}
	void ApplicationData::setWindowWidth(int w)
	{
		windowWidth = w; windowWidthHalf = w / 2;
	}
	void ApplicationData::setWindowHeight(int h)
	{
		windowHeight = h; windowHeightHalf = h / 2;
	}
	int ApplicationData::getWindowWidth()
	{
		return windowWidth;
	}
	int ApplicationData::getWindowHeight()
	{
		return windowHeight;
	}
	int ApplicationData::getWindowWidthHalf()
	{
		return windowWidthHalf;
	}
	int ApplicationData::getWindowHeightHalf()
	{
		return windowHeightHalf;
	}
	float ApplicationData::getMasterVolume()
	{
		return masterVolume;
	}
}