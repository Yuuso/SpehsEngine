#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/WriteBuffer.h"


namespace se
{
	// Write a pointer with an abstract serializable type
	template<typename Pointer>
	struct WriteAbstract
	{
		WriteAbstract(const Pointer& _t)
			: pointer(_t)
		{

		}
		void write(se::WriteBuffer& writeBuffer) const
		{
			const Pointer::element_type::SerialType serialType = pointer ? pointer->getSerialType() : Pointer::element_type::serialTypeNone;
			writeBuffer.write(serialType);
			if (serialType != Pointer::element_type::serialTypeNone)
			{
				writeBuffer.write(*pointer);
			}
		}
		se::Archive write() const
		{
			se::Archive archive;
			const Pointer::element_type::SerialType serialType = pointer ? pointer->getSerialType() : Pointer::element_type::serialTypeNone;
			se_write_to_archive(archive, serialType);
			if (serialType != Pointer::element_type::serialTypeNone)
			{
				se_write_to_archive(archive, *pointer);
			}
			return archive;
		}
		const Pointer& pointer;
	};
}

#define se_write_abstract(p_WriteBuffer, p_Pointer) do { se_write(p_WriteBuffer, se::WriteAbstract(p_Pointer)); } while (false)
#define se_write_to_archive_abstract(p_Archive, p_Pointer) do { p_Archive.write(#p_Pointer, se::WriteAbstract(p_Pointer)); } while (false)
