#include "SpehsEngine/Core/Inivar.h"
#include "SpehsEngine/Core/Inifile.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/Log.h"

namespace spehs
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