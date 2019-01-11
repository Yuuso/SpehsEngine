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
		/* Writes this into another archive. */
		void write(Archive& archive, const std::string& valueName) const;
		/* Reads this from another archive. */
		bool read(const Archive& archive, const std::string& valueName);
		/* Must internally implement writing (assigning) WriteBuffer into the archive, to prevent endless recursion. */
		void write(const std::string& valueName, const se::WriteBuffer& writeBuffer);
		bool read(const std::string& valueName, se::WriteBuffer& writeBuffer) const;

		/* Const class, has const write */
		template<class T>
		typename std::enable_if<has_write<T, void(T::*)(Archive&, const std::string&) const>::value, void>::type write(const std::string& valueName, const T& value)
		{
			value.write(*this, valueName);
		}
		/* Const class, doesn't have const write */
		template<class T>
		typename std::enable_if<!has_write<T, void(T::*)(Archive&, const std::string&) const>::value, void>::type write(const std::string& valueName, const T& value)
		{
			writeToArchive(*this, valueName, value);
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

		/* Is class, has mutable read */
		template<class T>
		typename std::enable_if<has_read<T, bool(T::*)(const Archive&, const std::string&)>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			return value.read(*this, valueName);
		}
		/* Is class, doesn't have mutable read */
		template<class T>
		typename std::enable_if<!has_read<T, bool(T::*)(const Archive&, const std::string&)>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			return readFromArchive(*this, valueName, value);
		}
		/* Reads a non-class value from the archive. Returns false if value is found but read fails. */
		template<typename T>
		typename std::enable_if<!std::is_class<T>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			const std::unordered_map<uint32_t, WriteBuffer>::const_iterator it = data.find(getDataHash(typeid(T).name(), valueName));
			if (it != data.end())
			{
				ReadBuffer readBuffer(it->second[0], it->second.getSize());
				return readBuffer.read(value);
			}
			else
			{
				return true;
			}
		}
		
	private:

		/* Returns a platform independent 64 bit hash from two given strings. */
		uint32_t getDataHash(const std::string& typeName, const std::string& valueName) const;

		std::unordered_map<uint32_t, WriteBuffer> data;
	};
}

#define se_write_to_archive(p_Archive, p_Value) do { p_Archive.write(#p_Value, p_Value); } while(false)
#define se_read_from_archive(p_Archive, p_Value) do { if (!p_Archive.read(#p_Value, p_Value)) return false; } while(false)
