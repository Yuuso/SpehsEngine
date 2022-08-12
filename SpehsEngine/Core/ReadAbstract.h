#pragma once

#include "SpehsEngine/Core/ReadBuffer.h"


namespace se
{
	// Read a pointer with an abstract serializable type
	template<typename Pointer>
	class ReadBufferAbstract
	{
	public:
		ReadBufferAbstract(Pointer& _t)
			: pointer(_t)
		{
		}
		bool read(ReadBuffer& readBuffer)
		{
			Pointer::element_type::SerialType serialType = Pointer::element_type::serialTypeNone;
			se_read(readBuffer, serialType);
			if (serialType != Pointer::element_type::serialTypeNone)
			{
				pointer.reset(Pointer::element_type::allocate(serialType).release());
				se_assert(pointer);
				if (pointer)
				{
					return readImpl(readBuffer, *pointer);
				}
				else
				{
					return false;
				}
			}
			return true;
		}
	private:
		// Has support for ReadBuffer
		template<typename T>
		typename std::enable_if<ReadBuffer::has_member_read<T, bool(T::*)(ReadBuffer&)>::value || ReadBuffer::has_free_read<T>::value, bool>::type
			readImpl(ReadBuffer& readBuffer, T& t) const
		{
			return readBuffer.read(t);
		}
		// No support for ReadBuffer but supports Archive
		template<typename T>
		typename std::enable_if<!ReadBuffer::has_member_read<T, bool(T::*)(ReadBuffer&)>::value && !ReadBuffer::has_free_read<T>::value
			&& (Archive::has_member_read<T, bool(T::*)(const Archive&)>::value || Archive::has_free_read<T>::value), bool>::type
			readImpl(ReadBuffer& readBuffer, T& t) const
		{
			Archive archive;
			readBuffer.read(archive);
			return t.read(archive);
		}
		Pointer& pointer;
	};
	template<typename Pointer>
	struct ReadArchiveAbstract
	{
		ReadArchiveAbstract(Pointer& _t)
			: pointer(_t)
		{
		}
		bool read(const se::Archive& archive)
		{
			Pointer::element_type::SerialType serialType = Pointer::element_type::serialTypeNone;
			se_read_from_archive(archive, serialType);
			if (serialType != Pointer::element_type::serialTypeNone)
			{
				pointer.reset(Pointer::element_type::allocate(serialType).release());
				se_assert(pointer);
				if (pointer)
				{
					se_read_from_archive(archive, *pointer);
				}
				else
				{
					return false;
				}
			}
			return true;
		}
		Pointer& pointer;
	};
}

#define se_read_abstract(p_ReadBuffer, p_Pointer) do { se::ReadBufferAbstract p_readAbstract(p_Pointer); se_read(p_ReadBuffer, p_readAbstract); } while (false)
#define se_read_from_archive_abstract(p_Archive, p_Pointer) do { se::ReadArchiveAbstract p_readAbstract(p_Pointer); if (!p_Archive.read(#p_Pointer, p_readAbstract)) return false; } while (false)
