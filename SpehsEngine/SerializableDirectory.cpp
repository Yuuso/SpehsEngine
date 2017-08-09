#include "Exceptions.h"
#include "SerializableDirectory.h"


namespace spehs
{
	SerializableDirectory::SerializableDirectory() : parent(nullptr)
	{

	}
	SerializableDirectory::SerializableDirectory(std::string _name) : parent(nullptr), name(_name)
	{
		if (name.size() == 0)
			spehs::exceptions::unexpectedError(__FUNCTION__" Serializable directory name cannot be null!");
	}
	SerializableDirectory::SerializableDirectory(const SerializableDirectory& other)
	{
		spehs::exceptions::warning(__FUNCTION__": TODO");
	}
	SerializableDirectory::~SerializableDirectory()
	{
		for (unsigned i = 0; i < data.size(); i++)
			delete data[i];
	}
	SerializableDirectory& SerializableDirectory::operator=(const SerializableDirectory& other)
	{
		//TODO2
		return *this;
	}
	size_t SerializableDirectory::packetSize() const
	{
		size_t bytes = sizeof(unsigned/*name char count*/) + sizeof(char) * name.size() + sizeof(unsigned/*child count*/);
		for (unsigned i = 0; i < subDirectories.size(); i++)
			bytes += subDirectories[i].packetSize();
		return bytes;
	}
	size_t SerializableDirectory::write(unsigned char* buffer) const
	{
		size_t offset(0);
		unsigned count(name.size());
		memcpy(&buffer[offset], &count, sizeof(count));							offset += sizeof(count);
		if (count > 0)
		{
			memcpy(&buffer[offset], name.data(), sizeof(char) * count);			offset += sizeof(char) * count;
		}
		count = subDirectories.size();
		memcpy(&buffer[offset], &count, sizeof(count));							offset += sizeof(count);
		for (unsigned i = 0; i < subDirectories.size(); i++)
			offset += subDirectories[i].write(&buffer[offset]);
		return offset;
	}
	size_t SerializableDirectory::read(const unsigned char* buffer)
	{
		size_t offset(0);
		unsigned count;
		memcpy(&count, &buffer[offset], sizeof(count));							offset += sizeof(count);
		name.resize(count);
		if (count > 0)
		{
			memcpy(&name[0], &buffer[offset], sizeof(char) * count);			offset += sizeof(char) * count;
		}
		memcpy(&count, &buffer[offset], sizeof(count));							offset += sizeof(count);
		subDirectories.resize(count);
		for (unsigned i = 0; i < subDirectories.size(); i++)
		{
			subDirectories[i].parent = this;
			offset += subDirectories[i].read(&buffer[offset]);
		}
		return offset;
	}
	bool SerializableDirectory::deleteSubDirectory(const std::string _name)
	{
		for (unsigned i = 0; i < subDirectories.size(); i++)
		{
			if (subDirectories[i].name == _name)
			{
				subDirectories.erase(subDirectories.begin() + i);
				return true;
			}
		}
		return false;
	}
	bool SerializableDirectory::rename(const std::string newName)
	{
		if (newName.size() == 0)
		{
			spehs::exceptions::warning("Serializable directory name cannot be set to null!");
			return false;
		}
		if (parent)
		{//Check sibling names for dublicate
			for (unsigned i = 0; i < parent->subDirectories.size(); i++)
			{
				if (parent->subDirectories[i].name == newName)
				{
					spehs::exceptions::warning("Serializable directory name cannot be set to same as sibling directory name!");
					return false;
				}
			}
		}
		name = newName;
		return true;
	}
	SerializableDirectory* SerializableDirectory::createSubDirectory(const std::string _name)
	{
		if (_name.size() == 0)
		{
			spehs::exceptions::warning(__FUNCTION__" Serializable directory name cannot be null!");
			return nullptr;
		}
		for (unsigned i = 0; i < subDirectories.size(); i++)
		{
			if (subDirectories[i].name == _name)
				return nullptr;//Name already exists!
		}
		subDirectories.resize(subDirectories.size() + 1);
		subDirectories.back().parent = this;
		subDirectories.back().name = _name;
		return &subDirectories.back();
	}
	SerializableDirectory* SerializableDirectory::getDirectory(std::string directoryPath)
	{
		for (unsigned p = 0; p < directoryPath.size(); p++)
		{
			if (directoryPath[p] == '/' || directoryPath[p] == '\\')
			{//Looking for sub directory

				//Check if this directory matches the first directory in the directory path
				std::string firstParentDirectory;
				if (p == 0)
				{
					spehs::exceptions::warning("Provided directory path contains a null directory name! Invalid path: \"" + directoryPath + "\"");
					return nullptr;
				}
				firstParentDirectory.resize(p);
				for (unsigned c = 0; c < p; c++)
					firstParentDirectory[c] = directoryPath[c];
				if (name != firstParentDirectory)
					return nullptr;//This is not the directory being searched for

				//Is sub directory, search below
				std::string subPath;
				subPath.resize(directoryPath.size() - p - 1);
				if (subPath.size() == 0)
				{
					spehs::exceptions::warning("Specified sub directory name is null! Invalid path: Invalid path: \"" + directoryPath + "\"");
					return nullptr;
				}
				memcpy(&subPath[0], &directoryPath[p + 1], sizeof(char) * subPath.size());
				for (unsigned s = 0; s < subDirectories.size(); s++)
				{
					SerializableDirectory* searchResult(subDirectories[s].getDirectory(subPath));
					if (searchResult)
						return searchResult;
				}
				return nullptr;
			}
		}
		return directoryPath == name ? this : nullptr;
	}
	std::string SerializableDirectory::getPath() const
	{
		if (parent)
			return parent->getPath() + '/' + name;
		else
			return name;
	}
}