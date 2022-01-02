#pragma once

#include "SpehsEngine/Core/ReadBuffer.h"


namespace se
{
	// Read a pointer with an abstract serializable type
	template<typename Pointer>
	struct ReadAbstract
	{
		ReadAbstract(Pointer& _t)
			: pointer(_t)
		{
		}
		bool read(se::ReadBuffer& readBuffer)
		{
			Pointer::element_type::SerialType serialType = Pointer::element_type::serialTypeNone;
			se_read(readBuffer, serialType);
			if (serialType != Pointer::element_type::serialTypeNone)
			{
				pointer.reset(Pointer::element_type::allocate(serialType).release());
				se_assert(pointer);
				if (pointer)
				{
					se_read(readBuffer, *pointer);
				}
				else
				{
					return false;
				}
			}
			return true;
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

#define se_read_abstract(p_ReadBuffer, p_Pointer) do { se::ReadAbstract p_readAbstract(p_Pointer); se_read(p_ReadBuffer, p_readAbstract); } while (false)
#define se_read_from_archive_abstract(p_Archive, p_Pointer) do { se::ReadAbstract p_readAbstract(p_Pointer); if (!p_Archive.read(#p_Pointer, p_readAbstract)) return false; } while (false)
