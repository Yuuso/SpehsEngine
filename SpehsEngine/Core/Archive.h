#pragma once
#include <string>
#include <unordered_map>
#include "SpehsEngine/Core/HasMemberFunction.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	/*
		Stores values in an unordered format.
		Allows class data to be easily stored between versions (format changes).
		Archive is not guaranteed to be platform independent - only the platform that wrote the archive can read it.
		You can write fundamental types, write buffers and (sub) archives into an archive.
	*/
	class Archive
	{
	public:
		SPEHS_HAS_MEMBER_FUNCTION(write, has_write);
		SPEHS_HAS_MEMBER_FUNCTION(read, has_read);
	public:

		Archive();

		void clear();

		/* Writes archive contents into a buffer. */
		void write(WriteBuffer& writeBuffer) const;
		/* Clears and reads archive contents from a buffer. */
		bool read(ReadBuffer& readBuffer);

		/* Writes an archive into this. */
		void write(const std::string& valueName, const Archive& archive);
		/* Reads an archive from this. */
		bool read(const std::string& valueName, Archive& archive) const;

		/*
			Writes (assigns) a write buffer into the archive.
			Must internally implement writing (assigning) WriteBuffer into the archive, to prevent endless recursion.
		*/
		void write(const std::string& valueName, const se::WriteBuffer& writeBuffer);
		/* Reads (assigns) a write buffer from the archive. */
		bool read(const std::string& valueName, se::WriteBuffer& writeBuffer) const;

		/* Creates an archive from a class with the write member function. */
		template<class T>
		static typename std::enable_if<std::is_class<T>::value && has_write<T, Archive(T::*)() const>::value, Archive>::type create(const T& value)
		{
			return value.write();
		}
		/* Creates an archive from a class without the write member function. */
		template<class T>
		static typename std::enable_if<std::is_class<T>::value && !has_write<T, Archive(T::*)() const>::value, Archive>::type create(const T& value)
		{
			return writeToArchive(value);
		}

		/* Writes a non-class value into the archive. */
		template<typename T>
		typename std::enable_if<!std::is_class<T>::value, void>::type write(const std::string& valueName, const T& value)
		{
			const uint32_t hash = getDataHash(typeid(T).name(), valueName);
			se_assert(data.find(hash) == data.end());
			WriteBuffer& writeBuffer = data[hash];
			writeBuffer.write(value);
		}
		/* Reads a non-class value from the archive. Returns false if value is found but read fails. */
		template<typename T>
		typename std::enable_if<!std::is_class<T>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			const std::unordered_map<uint32_t, WriteBuffer>::const_iterator it = data.find(getDataHash(typeid(T).name(), valueName));
			if (it != data.end())
			{
				if (it->second.getSize() > 0)
				{
					ReadBuffer readBuffer(it->second[0], it->second.getSize());
					return readBuffer.read(value);
				}
				else
				{
					ReadBuffer readBuffer(nullptr, 0);
					return readBuffer.read(value);
				}
			}
			else
			{
				return true;
			}
		}

		/* Is class, has const write. Write a sub archive and write that into this. */
		template<class T>
		typename std::enable_if<!std::is_same<typename std::remove_cv<T>::type, Archive>::value && !std::is_same<typename std::remove_cv<T>::type, WriteBuffer>::value && std::is_class<T>::value
			&& has_write<T, Archive(T::*)() const>::value, void>::type write(const std::string& valueName, const T& value)
		{
			const se::Archive archive = value.write();
			write(valueName, archive);
		}
		/* Is class, doesn't have const write. Read a sub archive from this and read the value from the sub archive. */
		template<class T>
		typename std::enable_if<!std::is_same<typename std::remove_cv<T>::type, Archive>::value && !std::is_same<typename std::remove_cv<T>::type, WriteBuffer>::value && std::is_class<T>::value
			&& !has_write<T, Archive(T::*)() const>::value, void>::type write(const std::string& valueName, T& value)
		{
			const se::Archive archive = writeToArchive(value);
			write(valueName, archive);
		}

		/* Is class, has mutable read */
		template<class T>
		typename std::enable_if<has_read<T, bool(T::*)(const Archive&)>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			se::Archive archive;
			read(valueName, archive);
			return value.read(archive);
		}
		/* Is class, doesn't have mutable read */
		template<class T>
		typename std::enable_if<!has_read<T, bool(T::*)(const Archive&)>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			se::Archive archive;
			read(valueName, archive);
			return readFromArchive(archive, value);
		}
		
	private:

		/* Returns a platform independent 64 bit hash from two given strings. */
		uint32_t getDataHash(const std::string& typeName, const std::string& valueName) const;

		std::unordered_map<uint32_t, WriteBuffer> data;
	};
}

#define se_write_to_archive(p_Archive, p_Value) do { p_Archive.write(#p_Value, p_Value); } while(false)
#define se_read_from_archive(p_Archive, p_Value) do { if (!p_Archive.read(#p_Value, p_Value)) return false; } while(false)
