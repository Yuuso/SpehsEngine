#include "stdafx.h"
#include "SpehsEngine/Core/Inivar.h"
#include "SpehsEngine/Core/Inifile.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/Log.h"

namespace se
{
	AbstractInivar::AbstractInivar(Inisection& _section, const std::string& _name, const std::string& _type)
		: name(_name)
		, type(_type)
		, section(_section)
	{
	}

	AbstractInivar::~AbstractInivar()
	{
	}

	void AbstractInivar::markForUnwrittenChanges()
	{
		std::lock_guard<std::recursive_mutex> lock(section.file.mutex);
		section.file.unwrittenChanges = true;
	}

	bool AbstractInivar::tryRetrieveValue()
	{
		Inisection::ReadVar* readVar = section.findReadVar(type, name);
		if (readVar)
		{
			if (fromString(readVar->value))
			{
				return true;
			}
			else
			{
				se::log::warning("Appvar::fromString() failed. Name: '" + readVar->name + "', value: '" + readVar->value + "'.");
				return false;
			}
		}
		else
		{
			return false;
		}
	}


	//T = std::string
	template<>
	std::string Inivar<std::string>::toString() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return value;
	}
	template<>
	bool Inivar<std::string>::fromString(const std::string& string)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		value = string;
		return true;
	}

#define INIVAR_DEFAULT_STRING_TEMPLATE_FUNCTIONS(Type) \
	template<> \
	std::string Inivar<Type>::toString() const \
	{ \
		std::lock_guard<std::recursive_mutex> lock(mutex); \
		return std::to_string(value); \
	} \
	template<> \
	bool Inivar<Type>::fromString(const std::string& string) \
	{ \
		std::lock_guard<std::recursive_mutex> lock(mutex); \
		value = Type(std::atoi(string.c_str())); \
		return true; \
	}

	INIVAR_DEFAULT_STRING_TEMPLATE_FUNCTIONS(uint32_t)
	INIVAR_DEFAULT_STRING_TEMPLATE_FUNCTIONS(int32_t)
	INIVAR_DEFAULT_STRING_TEMPLATE_FUNCTIONS(uint16_t)
	INIVAR_DEFAULT_STRING_TEMPLATE_FUNCTIONS(int16_t)
	INIVAR_DEFAULT_STRING_TEMPLATE_FUNCTIONS(uint8_t)
	INIVAR_DEFAULT_STRING_TEMPLATE_FUNCTIONS(int8_t)



	//T = float
	template<>
	std::string Inivar<float>::toString() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return std::to_string(value);
	}
	template<>
	bool Inivar<float>::fromString(const std::string& string)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		value = std::stof(string.c_str());
		return true;
	}



	//T = bool
	template<>
	std::string Inivar<bool>::toString() const
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		return std::to_string(value);
	}
	template<>
	bool Inivar<bool>::fromString(const std::string& string)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (string == "0" || string == "false")
		{
			value = false;
			return true;
		}
		else if (string == "1" || string == "true")
		{
			value = true;
			return true;
		}
		else
		{
			return false;
		}
	}
}