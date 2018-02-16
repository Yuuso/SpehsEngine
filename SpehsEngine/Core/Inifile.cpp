#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/FileStream.h"
#include "SpehsEngine/Core/Inifile.h"
#include "SpehsEngine/Core/Log.h"

namespace spehs
{
	const std::string Inifile::fileExtension(".ini");
	Inifile::Inifile(const std::string& _name)
		: name(_name)
	{

	}

	Inifile::~Inifile()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		for (size_t i = 0; i < sections.size(); i++)
			delete sections[i];
		sections.clear();
	}

	void Inifile::update()
	{
		//std::lock_guard<std::recursive_mutex> lock(mutex);
		if (hasUnwrittenChanges())
		{
			write();
		}
	}

	bool Inifile::write()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		std::ofstream stream(name + fileExtension);
		if (stream.fail())
		{
			log::error("Failed to write inifile data! Inifile name: " + name);
			return false;
		}

		std::string outputString;
		for (size_t s = 0; s < sections.size(); s++)
		{
			if (!sections[s]->vars.empty())
			{
				outputString += "[" + sections[s]->name + "]\n";
				for (size_t v = 0; v < sections[s]->vars.size(); v++)
					outputString += sections[s]->vars[v]->type + " " + sections[s]->vars[v]->name + " = " + sections[s]->vars[v]->toString() + "\n";
			}
		}
		if (outputString.size() > 0)
			outputString.pop_back();//Remove the last newline
		stream << outputString;

		stream.close();
		unwrittenChanges = false;
	}

	bool Inifile::read()
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
				log::error("Could not write application data! Failed to read application data!");
				return false;
			}
			stream.open(name + fileExtension);
			if (stream.fail())
			{
				log::error("Failed to read Application data!");
				return false;
			}
		}

		Inisection* section = nullptr;
		std::string line;
		while (!stream.eof())
		{
			std::getline(stream, line);
			if (line.empty())
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

				//Format:
				//(unsigned) type name (name2) (nameN) = value
				std::vector<std::string> words;
				getWords(line, words);
				const bool unsignedType = words.size() > 0 && words[0] == "unsigned";
				const int wordsMinCount = (unsignedType ? 1 : 0)/*(unsigned)*/ + 4/*type + name + assignment + value*/;
				if (words.size() >= wordsMinCount)
				{
					//Assignment word index
					const size_t invalidAssignmentIndex = ~0;
					size_t assignmentIndex = invalidAssignmentIndex;
					for (size_t i = 0; i < words.size(); i++)
					{
						if (words[i] == "=")
						{
							assignmentIndex = i;
							break;
						}
					}

					if (assignmentIndex != invalidAssignmentIndex)
					{
						//Type
						const std::string type = unsignedType ? "unsigned " + words[1] : words[0];

						//Name
						std::string name;
						const size_t nameBeginIndex = unsignedType ? 2 : 1;
						for (size_t i = nameBeginIndex; i < assignmentIndex; i++)
							name += " " + words[i];
						if (name.size() > 1)
						{
							//Erase first space from the name
							name.erase(name.begin());

							//Value
							std::string value;
							for (size_t i = assignmentIndex + 1; i < words.size(); i++)
								value += " " + words[i];

							if (value.size() > 1)
							{
								//Erase first space from the value
								value.erase(value.begin());

								if (section)
								{
									bool exists = false;
									for (size_t i = 0; i < section->readVars.size(); i++)
									{
										if (section->readVars[i].type == type && section->readVars[i].name == name)
										{
											spehs::log::warning("An appvar was read but the section already has an appvar with this type and name, discarding. Type: '" + type + "', Name: '" + name + "', Value: '" + value + "'.");
											exists = true;
											break;
										}
									}
									if (!exists)
										section->readVars.push_back(Inisection::ReadVar(type, name, value));
								}
								else
									spehs::log::warning("An appvar was read but no section was assigned. Type: '" + type + "', Name: '" + name + "', Value: '" + value + "'.");
							}
						}
					}
				}
			}
		}

		stream.close();
	}

	bool Inifile::hasUnwrittenChanges()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return unwrittenChanges;
	}

	Inisection& Inifile::getSection(const std::string& name)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		for (size_t s = 0; s < sections.size(); s++)
		{
			if (sections[s]->name == name)
			{
				return *sections[s];
			}
		}
		sections.push_back(new Inisection(*this, name));
		unwrittenChanges = true;
		return *sections.back();
	}


	Inisection::Inisection(Inifile& _file, const std::string& _name)
		: file(_file), name(_name)
	{
	}

	Inisection::~Inisection()
	{
	}
	
	Inisection::ReadVar* Inisection::findReadVar(const std::string& type, const std::string& name)
	{
		for (size_t i = 0; i < readVars.size(); i++)
		{
			if (readVars[i].type == type && readVars[i].name == name)
				return &readVars[i];
		}
		return nullptr;
	}
}