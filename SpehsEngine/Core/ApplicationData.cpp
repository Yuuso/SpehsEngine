#include <algorithm>
#include <string>
#include <cstring>
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/FileStream.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Log.h"

namespace spehs
{
	//DATA
	//public
	int ApplicationData::fullscreen = 0;
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
		stream << "Fullscreen=" << fullscreen << "\n";
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
		readValueIntoInt(stream, windowWidth);				windowWidth = std::max(1, windowWidth);			windowWidthHalf = windowWidth / 2;
		readValueIntoInt(stream, windowHeight);				windowHeight = std::max(1, windowHeight);		windowHeightHalf = windowHeight / 2;
		readValueIntoInt(stream, fullscreen);
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
		//audio::AudioEngine::updateGain();
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

	//void ApplicationData::setWindowWidth(int w)
	//{
	//	windowWidth = w;
	//	windowWidthHalf = w / 2;
	//}

	//void ApplicationData::setWindowHeight(int h)
	//{
	//	windowHeight = h;
	//	windowHeightHalf = h / 2;
	//}

	//int ApplicationData::getWindowWidth()
	//{
	//	return windowWidth;
	//}

	//int ApplicationData::getWindowHeight()
	//{
	//	return windowHeight;
	//}

	//int ApplicationData::getWindowWidthHalf()
	//{
	//	return windowWidthHalf;
	//}

	//int ApplicationData::getWindowHeightHalf()
	//{
	//	return windowHeightHalf;
	//}

	float ApplicationData::getMasterVolume()
	{
		return masterVolume;
	}
	

















	const std::string Appvars::fileExtension(".ini");
	Appvars::Appvars(const std::string& _name)
		: name(_name)
	{

	}

	bool Appvars::read()
	{
		if (name.size() == 0)
		{
			spehs::log::warning("Cannot write appvars, no fileName was specified.");
			return false;
		}

		//Create file stream
		std::ifstream stream(name + fileExtension);
		if (stream.fail())
		{//Application data does not exist
			if (!write())
			{
				spehs::exceptions::unexpectedError("Could not write application data! Failed to read application data!");
				return false;
			}
			stream.open(name + fileExtension);
			if (stream.fail())
			{
				spehs::exceptions::unexpectedError("Failed to read Application data!");
				return false;
			}
		}

		Section* section = nullptr;
		std::string line;
		while (true)
		{
			std::getline(stream, line);
			if (line.size() == 0)
				break;
			if (line.front() == '[' && line.back() == ']')
			{//Section
				if (line.size() > 2)
				{
					std::string name;
					name.resize(line.size() - 2);
					memcpy(&name[0], &line[1], name.size());
					section = &getSection(name);
				}
				else
				{
					spehs::log::warning(name + fileExtension + " contains an empty section name. No section created.");
				}
			}
			else
			{//Search for a name-value pair
				std::string name;
				std::string value;
				for (size_t i = 0; i < line.size(); i++)
				{
					if (line[i] == '=')
					{//Reached the name-value assignment
						if (name.size() == 0)
						{
							name.resize(i);
							memcpy(&name[0], &line[0], i);
						}
						else
						{
							name.clear();
							value.clear();
							break;
						}
					}
					else if (i == line.size() - 1)
					{//reached the last character of the
						if (name.size() > 0)
						{
							value.resize(i - name.size());
							memcpy(&value[0], &line[name.size() + 1], value.size());
						}
					}
				}
				if (name.size() > 0 && value.size() > 0)
				{
					if (section)
					{
						section->readAppvars.push_back(Section::ReadAppvar(name, value));
					}
					else
					{
						spehs::log::warning("An appvar was read but no section was assigned. Name: '" + name + "', value: '" + value + "'.");
					}
				}
			}
		}

		stream.close();
	}

	bool Appvars::write()
	{
		std::ofstream stream(name + fileExtension);
		if (stream.fail())
		{
			spehs::exceptions::unexpectedError("Failed to write application data!");
			return false;
		}

		std::string outputString;
		for (size_t s = 0; s < sections.size(); s++)
		{
			outputString += "[" + sections[s]->name + "]\n";
			for (size_t v = 0; v < sections[s]->appvarBases.size(); v++)
				outputString += sections[s]->appvarBases[v]->name + "=" + sections[s]->appvarBases[v]->toString() + "\n";
		}
		if (outputString.size() > 0)
			outputString.pop_back();//Remove the last newline
		stream << outputString;

		stream.close();
		unwrittenChanges = false;
	}

	bool Appvars::hasUnwrittenChanges()
	{
		return unwrittenChanges;
	}

	Appvars::Section& Appvars::getSection(const std::string& name)
	{
		for (size_t s = 0; s < sections.size(); s++)
		{
			if (sections[s]->name == name)
			{
				return *sections[s];
			}
		}
		sections.push_back(new Section(*this, name));
		return *sections.back();
	}

	Appvars::Section::~Section()
	{
		for (size_t i = 0; i < appvarBases.size(); i++)
		{
			SPEHS_ASSERT(appvarBases[i]->section == this);
			appvarBases[i]->section = nullptr;
		}
	}

	bool Appvars::Section::add(AppvarBase& appvar)
	{
		for (size_t i = 0; i < appvarBases.size(); i++)
		{
			if (appvarBases[i] == &appvar)
			{
				spehs::log::error("This appvar has already been added to this section: '" + appvar.name + "'.");
				return true;
			}
			if (appvarBases[i]->name == appvar.name)
			{
				spehs::log::error("An appvar with this name identifier already exists in this section: '" + appvar.name + "'.");
				return false;
			}
		}
		appvarBases.push_back(&appvar);
		
		//Check read values
		for (size_t i = 0; i < readAppvars.size(); i++)
		{
			if (readAppvars[i].name == appvar.name)
			{
				if (appvar.fromString(readAppvars[i].value))
				{
					readAppvars[i] = readAppvars.back();
					readAppvars.pop_back();
				}
				spehs::log::warning("Appvar::fromString() failed. Name: '" + readAppvars[i].name + "', value: '" + readAppvars[i].value + "'.");
				break;
			}
		}

		return true;
	}

	void Appvars::Section::remove(AppvarBase& appvar)
	{
		for (size_t i = 0; i < appvarBases.size(); i++)
		{
			if (appvarBases[i] == &appvar)
			{
				appvarBases.erase(appvarBases.begin() + i);
				return;
			}
		}
	}

	AppvarBase::AppvarBase(Appvars& _appvars, const std::string& _section, const std::string& _name)
		: section(&_appvars.getSection(_section))
		, name(_name)
	{
		if (section)
			section->add(*this);
	}

	AppvarBase::~AppvarBase()
	{
		if (section)
			section->remove(*this);
	}
}