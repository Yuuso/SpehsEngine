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

	Appvars::~Appvars()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		for (size_t i = 0; i < sections.size(); i++)
			delete sections[i];
		sections.clear();
	}

	void Appvars::update()
	{
		//std::lock_guard<std::recursive_mutex> lock(mutex);
		if (hasUnwrittenChanges())
		{
			write();
		}
	}

	bool Appvars::write()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		std::ofstream stream(name + fileExtension);
		if (stream.fail())
		{
			spehs::exceptions::unexpectedError("Failed to write appvar data! Appvars name: " + name);
			return false;
		}

		std::string outputString;
		for (size_t s = 0; s < sections.size(); s++)
		{
			outputString += "[" + sections[s]->name + "]\n";
			for (size_t v = 0; v < sections[s]->vars.size(); v++)
				outputString += sections[s]->vars[v]->type + " " + sections[s]->vars[v]->name + " = " + sections[s]->vars[v]->toString() + "\n";
		}
		if (outputString.size() > 0)
			outputString.pop_back();//Remove the last newline
		stream << outputString;

		stream.close();
		unwrittenChanges = false;
	}

	bool Appvars::read()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (name.size() == 0)
		{
			spehs::log::warning("Cannot write appvars, no fileName was specified.");
			return false;
		}
		
		//Clear all previously read appvars
		for (size_t i = 0; i < sections.size(); i++)
			sections[i]->readVars.clear();

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
		while (!stream.eof())
		{
			std::getline(stream, line);
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
				//type name = value
				std::string type;
				std::string name;
				std::string value;
				size_t valueBeginIndex = 0;
				if (line.size() >= 5/*line min size*/)
				{
					for (size_t i = 0; i < line.size(); i++)
					{
						if (line[i] == ' ' && type.empty())
						{//Reached type
							if (i > 0)
							{
								type.resize(i);
								memcpy(&type[0], &line[0], i);
							}
							else
							{
								type.clear();
								name.clear();
								value.clear();
								break;
							}
						}
						else if (line[i] == '=')
						{//Reached the name-value assignment
							if (name.empty())
							{
								const size_t nameBeginIndex = type.size() + 1;
								const size_t nameEndIndex = i - 1 - ((line[i - 1] == ' ') ? 1 : 0);
								SPEHS_ASSERT(nameEndIndex >= nameBeginIndex);
								const size_t nameLength = nameEndIndex - nameBeginIndex + 1;
								SPEHS_ASSERT(nameLength > 0);
								name.resize(nameLength);
								memcpy(&name[0], &line[nameBeginIndex], nameLength);
								if (line.size() - i - 1 > 0)
								{
									if (line[i + 1] == ' ')
										valueBeginIndex = i + 2;
									else
										valueBeginIndex = i + 1;
								}
								else
								{
									type.clear();
									name.clear();
									value.clear();
									break;
								}
							}
							else
							{
								type.clear();
								name.clear();
								value.clear();
								break;
							}
						}
						else if (i == line.size() - 1)
						{//reached the last character of the line
							if (valueBeginIndex == 0 || name.empty())
							{
								type.clear();
								name.clear();
								value.clear();
								break;
							}
							else
							{
								const size_t valueLength = i - valueBeginIndex + 1;
								value.resize(valueLength);
								memcpy(&value[0], &line[valueBeginIndex], valueLength);
							}
						}
					}
				}
				if (type.size() > 0 && name.size() > 0 && value.size() > 0)
				{
					if (section)
					{
						section->readVars.push_back(Section::ReadVar(type, name, value));
					}
					else
					{
						spehs::log::warning("An appvar was read but no section was assigned. Type: '" + type + "', Name: '" + name + "', Value: '" + value + "'.");
					}
				}
			}
		}

		stream.close();
	}

	bool Appvars::hasUnwrittenChanges()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return unwrittenChanges;
	}

	Appvars::Section& Appvars::getSection(const std::string& name)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		for (size_t s = 0; s < sections.size(); s++)
		{
			if (sections[s]->name == name)
			{
				return *sections[s];
			}
		}
		sections.push_back(new Section(*this, name));
		unwrittenChanges = true;
		return *sections.back();
	}

	Appvars::Section::~Section()
	{

	}
	
	Appvars::Section::ReadVar* Appvars::Section::findReadVar(const std::string& type, const std::string& name)
	{
		for (size_t i = 0; i < readVars.size(); i++)
		{
			if (readVars[i].type == type && readVars[i].name == name)
				return &readVars[i];
		}
		return nullptr;
	}

	Appvars::VarBase::VarBase(Section& _section, const std::string& _name, const std::string& _type)
		: name(_name)
		, type(_type)
		, section(_section)
	{
	}

	Appvars::VarBase::~VarBase()
	{
	}

	void Appvars::VarBase::markForUnwrittenChanges()
	{
		std::lock_guard<std::recursive_mutex> lock(section.appvars.mutex);
		section.appvars.unwrittenChanges = true;
	}

	bool Appvars::VarBase::tryRetrieveValue()
	{
		Appvars::Section::ReadVar* readVar = section.findReadVar(type, name);
		if (readVar)
		{
			if (fromString(readVar->value))
			{
				return true;
			}
			else
			{
				spehs::log::warning("Appvar::fromString() failed. Name: '" + readVar->name + "', value: '" + readVar->value + "'.");
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}