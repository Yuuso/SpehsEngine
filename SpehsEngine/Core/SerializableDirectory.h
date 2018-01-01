#pragma once
#include <vector>
#include <string>
#include "SpehsEngine/Core/Serializable.h"


namespace spehs
{
	/** Serializable directory is a serializable tree structure which contains data of serializable type.
	This meaning that all of the stored data must derive from the base class of spehs::Serializable.
	The directory can therefore easily be deconstructed and reconstructed into a stream.
	*/
	class SerializableDirectory : public Serializable
	{
		friend class std::allocator < SerializableDirectory > ;
		friend class std::vector < SerializableDirectory > ;
	public:
		SerializableDirectory();
		SerializableDirectory(const SerializableDirectory& other);
		~SerializableDirectory() override;
		SerializableDirectory& operator=(const SerializableDirectory& other);
		Serializable* clone() const override { return new SerializableDirectory(*this); }
		size_t packetSize() const override;
		size_t write(unsigned char* buffer) const override;
		size_t read(const unsigned char* buffer) override;

		//Setters
		bool deleteSubDirectory(const std::string _name);
		bool setName(const std::string newName);///< Returns true if name was successfully renamed. Rename will fail if name contains no characters or directory already contains a sibling with the same name
		SerializableDirectory* createSubDirectory(const std::string _name);
		//Getters
		SerializableDirectory* getDirectory(std::string directoryPath);//Returns directory based on given directory path if such directory exists
		SerializableDirectory* getParent() const { return parent; }
		SerializableDirectory* getRoot() { if (parent) return parent->getRoot(); return this; }
		std::string getPath() const;///< Returns full directory path, which includes every parent name separated from each other with a '/' character
		std::string getName() const { return name; }

		//Publicly visible attributes
		std::vector<Serializable*> data;

	private:
		std::string name;
		std::vector<SerializableDirectory> subDirectories;
		SerializableDirectory* parent;
	};
}