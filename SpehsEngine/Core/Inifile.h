#pragma once
#include "SpehsEngine/Core/Inivar.h"

namespace se
{
	class Inisection
	{
	public:
		Inisection(Inifile& _file, const std::string& _name);
		~Inisection();

		template<typename T>
		Inivar<T>& get(const std::string& _type, const std::string& _name, const T& _defaultValue);

		const std::string name;
		Inifile& file;
		std::vector<AbstractInivar*> vars;

		struct ReadVar
		{
			ReadVar(const std::string& _type, const std::string& _name, const std::string& _value) : type(_type), name(_name), value(_value) {}
			std::string type;
			std::string name;
			std::string value;
		};
		ReadVar* findReadVar(const std::string& type, const std::string& name);
		std::vector<ReadVar> readVars;
	};


	/*
		static manager class for reading and writing inivars.
		The inivars file is divided into sections.
		Each section consists of a group of inivars.
		Each inivar has a unque type + name combination in its section.
	*/
	class Inifile
	{
		friend class AbstractInivar;
		friend class Inisection;
	public:
		Inifile(const std::string& fileName);
		~Inifile();
		Inifile() = delete;
		Inifile(const Inifile& other) = delete;
		Inifile(const Inifile&& other) = delete;

		/* Writes to file if there are changes. */
		void update();
		/* This function can be used to query whether any inivars have changed since the last write. */
		bool hasUnwrittenChanges();
		/* Reads inivars from the file. */
		bool read();
		/* Writes all currently existing inivars into the file. */
		bool write();
		/* Returns a reference to an appvar. If var didn't exist before, it is added with the specified default value. */
		template<typename T>
		Inivar<T>& get(const std::string& section, const std::string& name, const T& defaultValue);
		
		/* This is the name for the file that resides in the working directory of the executable. */
		const std::string name;
		/* File extension, includes the dot. */
		static const std::string fileExtension;

	protected:
		Inisection& getSection(const std::string& name);

		mutable std::recursive_mutex mutex;
		bool unwrittenChanges;		
		std::vector<Inisection*> sections;
	};
	


	//TEMPLATE DEFINITIONS

	template<typename T>
	Inivar<T>& Inifile::get(const std::string& section, const std::string& name, const T& defaultValue)
	{
		const std::string type = typeid(T).name();
		Inivar<T>& var = getSection(section).get<T>(type, name, defaultValue);
		var.tryRetrieveValue();
		return var;
	}

	template<typename T>
	Inivar<T>& Inisection::get(const std::string& _type, const std::string& _name, const T& _defaultValue)
	{
		for (size_t i = 0; i < vars.size(); i++)
		{
			if (vars[i]->type == _type && vars[i]->name == _name)
				return (Inivar<T>&)(*vars[i]);
		}
		Inivar<T>* var = new Inivar<T>(*this, _name, _defaultValue);
		vars.push_back(var);
		file.unwrittenChanges = true;
		return *var;
	}
}