#pragma once
#include <vector>
#include <string>
#include "Serializable.h"


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
		SerializableDirectory(std::string _name);
		~SerializableDirectory() override;
		Serializable* newMyType() const override { return new SerializableDirectory(nullptr); }
		size_t packetSize() const override;
		size_t write(unsigned char* buffer) const override;
		size_t read(const unsigned char* buffer) override;

		//Setters
		bool deleteSubDirectory(const std::string _name);
		bool rename(const std::string newName);///< Returns true if name was successfully renamed. Rename will fail if name contains no characters or directory already contains a sibling with the same name
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
		SerializableDirectory();//Hidden default constructor
		std::string name;
		std::vector<SerializableDirectory> subDirectories;
		SerializableDirectory* parent;
	};
}