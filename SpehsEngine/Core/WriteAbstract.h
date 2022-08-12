#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/WriteBuffer.h"


namespace se
{
	// Write a pointer with an abstract serializable type
	template<typename Pointer>
	class WriteBufferAbstract
	{
	public:
		WriteBufferAbstract(const Pointer& _t)
			: pointer(_t)
		{
		}
		void write(WriteBuffer& writeBuffer) const
		{
			const Pointer::element_type::SerialType serialType = pointer ? pointer->getSerialType() : Pointer::element_type::serialTypeNone;
			writeBuffer.write(serialType);
			if (serialType != Pointer::element_type::serialTypeNone)
			{
				writeImpl<Pointer::element_type>(writeBuffer);
			}
		}
	private:
		// Has support for WriteBuffer
		template<typename T>
		typename std::enable_if<WriteBuffer::has_member_write<T, void(T::*)(WriteBuffer&) const>::value || WriteBuffer::has_free_write<T>::value, void>::type
			writeImpl(WriteBuffer& writeBuffer) const
		{
			writeBuffer.write(*pointer);
		}
		// No support for WriteBuffer but supports Archive
		template<typename T>
		typename std::enable_if<!WriteBuffer::has_member_write<T, void(T::*)(WriteBuffer&) const>::value && !WriteBuffer::has_free_write<T>::value
			&& (Archive::has_member_write<T, Archive(T::*)() const>::value || Archive::has_free_write<T>::value), void>::type
			writeImpl(WriteBuffer& writeBuffer) const
		{
			Archive archive = pointer->write();
			writeBuffer.write(archive);
		}
		const Pointer& pointer;
	};
	template<typename Pointer>
	class WriteArchiveAbstract
	{
	public:
		WriteArchiveAbstract(const Pointer& _t)
			: pointer(_t)
		{
		}
		Archive write() const
		{
			Archive archive;
			const Pointer::element_type::SerialType serialType = pointer ? pointer->getSerialType() : Pointer::element_type::serialTypeNone;
			se_write_to_archive(archive, serialType);
			if (serialType != Pointer::element_type::serialTypeNone)
			{
				se_write_to_archive(archive, *pointer);
			}
			return archive;
		}
	private:
		const Pointer& pointer;
	};
}

#define se_write_abstract(p_WriteBuffer, p_Pointer) do { se_write(p_WriteBuffer, se::WriteBufferAbstract(p_Pointer)); } while (false)
#define se_write_to_archive_abstract(p_Archive, p_Pointer) do { p_Archive.write(#p_Pointer, se::WriteArchiveAbstract(p_Pointer)); } while (false)
