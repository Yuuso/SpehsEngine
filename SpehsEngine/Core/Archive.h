#pragma once
#include <string>
#include <unordered_map>
#include "SpehsEngine/Core/HasMemberFunction.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"

namespace se
{
	/*
		Stores values in an unordered format.
		Allows class data to be easily stored between versions (format changes).
		Archive is platform independent.
		You can write fundamental types, write buffers and (sub) archives into an archive.
		A user defined type does not have to implement archive write/read methods/free functions, in which case Write/ReadBuffer write/read methods/free functions are used.
	*/
	class Archive;
	class Archive
	{
	public:
		SPEHS_HAS_MEMBER_FUNCTION(write, has_member_write);
		SPEHS_HAS_MEMBER_FUNCTION(read, has_member_read);
		template<class> struct type_sink { typedef void type; }; // consumes a type, and makes it `void`
		template<class T> using type_sink_t = typename type_sink<T>::type;
		template<class T, class = void> struct has_free_write : std::false_type {};
		template<class T> struct has_free_write<T, type_sink_t<decltype(writeToArchive(std::declval<const T&>()), std::declval<Archive>())>> : std::true_type {};
		template<class T, class = void> struct has_free_read : std::false_type {};
		template<class T> struct has_free_read<T, type_sink_t<decltype(readFromArchive(std::declval<const Archive&>(), std::declval<T&>()), bool())>> : std::true_type {};
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

		/* Is class, has member write. Write a sub archive and write that into this. */
		template<class T>
		typename std::enable_if<std::is_class<T>::value && !std::is_same<typename std::remove_cv<T>::type, Archive>::value && !std::is_same<typename std::remove_cv<T>::type, WriteBuffer>::value
			&& has_member_write<T, Archive(T::*)() const>::value, void>::type write(const std::string& valueName, const T& value)
		{
			const Archive archive = value.write();
			write(valueName, archive);
		}
		/* Is class, has free write. Read a sub archive from this and read the value from the sub archive. */
		template<class T>
		typename std::enable_if<std::is_class<T>::value && !std::is_same<typename std::remove_cv<T>::type, Archive>::value && !std::is_same<typename std::remove_cv<T>::type, WriteBuffer>::value
			&& !has_member_write<T, Archive(T::*)() const>::value
			&& has_free_write<T>::value, void>::type write(const std::string& valueName, const T& value)
		{
			const Archive archive = writeToArchive(value);
			write(valueName, archive);
		}
		/* Is class, doesn't have any archive methods but has write buffer method. */
		template<class T>
		typename std::enable_if<std::is_class<T>::value && !std::is_same<typename std::remove_cv<T>::type, Archive>::value && !std::is_same<typename std::remove_cv<T>::type, WriteBuffer>::value
			&& !has_member_write<T, Archive(T::*)() const>::value
			&& !has_free_write<T>::value
			&& has_member_write<T, void(T::*)(WriteBuffer&) const>::value, void>::type write(const std::string& valueName, const T& value)
		{
			WriteBuffer writeBuffer;
			value.write(writeBuffer);
			write(valueName, writeBuffer);
		}
		/* Is class, doesn't have any archive methods but has write buffer free function. */
		template<class T>
		typename std::enable_if<std::is_class<T>::value && !std::is_same<typename std::remove_cv<T>::type, Archive>::value && !std::is_same<typename std::remove_cv<T>::type, WriteBuffer>::value
			&& !has_member_write<T, Archive(T::*)() const>::value && !has_free_write<T>::value
			&& !has_member_write<T, void(T::*)(WriteBuffer&) const>::value && WriteBuffer::has_free_write<T>::value, void>::type write(const std::string& valueName, const T& value)
		{
			WriteBuffer writeBuffer;
			writeToBuffer(writeBuffer, value);
			write(valueName, writeBuffer);
		}

		/* Is class, has member read. */
		template<class T>
		typename std::enable_if<has_member_read<T, bool(T::*)(const Archive&)>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			Archive archive;
			read(valueName, archive);
			return value.read(archive);
		}
		/* Is class, doesn't have member read but has free read. */
		template<class T>
		typename std::enable_if<!has_member_read<T, bool(T::*)(const Archive&)>::value && has_free_read<T>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			Archive archive;
			read(valueName, archive);
			return readFromArchive(archive, value);
		}
		/* Is class, doesn't have any archive methods but has read buffer method. */
		template<class T>
		typename std::enable_if<!has_member_read<T, bool(T::*)(const Archive&)>::value && !has_free_read<T>::value
			&& ReadBuffer::has_member_read<T, bool(T::*)(ReadBuffer&)>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			WriteBuffer writeBuffer;
			if (!read(valueName, writeBuffer))
			{
				return false;
			}
			if (writeBuffer.getSize() > 0)
			{
				ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
				return value.read(readBuffer);
			}
			else
			{
				return true;
			}
		}
		/* Is class, doesn't have any archive methods but has read buffer free function. */
		template<class T>
		typename std::enable_if<!has_member_read<T, bool(T::*)(const Archive&)>::value && !has_free_read<T>::value
			&& !ReadBuffer::has_member_read<T, bool(T::*)(ReadBuffer&)>::value && ReadBuffer::has_free_read<T>::value, bool>::type read(const std::string& valueName, T& value) const
		{
			WriteBuffer writeBuffer;
			if (!read(valueName, writeBuffer))
			{
				return false;
			}
			if (writeBuffer.getSize() > 0)
			{
				ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
				return readFromBuffer(readBuffer, value);
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
