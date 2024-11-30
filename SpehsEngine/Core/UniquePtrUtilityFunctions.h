#pragma once

#include "SpehsEngine/Core/Serial/SerialUtility.h"


namespace se
{
	struct SerialTagUniquePtr {};
	template<typename T> struct SerialTag<std::unique_ptr<T>> { typedef SerialTagUniquePtr type; };
	template<> template<typename S, typename T>
	static bool Serial<SerialTagUniquePtr>::serial(S& _serial, T _value)
	{
		using ElementType = typename remove_cvref<T>::type::element_type;
		if constexpr (std::is_class<ElementType>::value)
		{
			if constexpr (S::getWritingEnabled())
			{
				// Write class
				const bool exists = _value.get() != nullptr;
				se_serial(_serial, exists, "exists");
				if (exists)
				{
					// NOTE: if there was a way to get this type from Serial<ElementType>::identify(), that would be great.
					using SerialId = typename SerialId<ElementType>::type;
					const SerialId serialId = Serial<ElementType>::template identify<SerialId>(*_value);
					se_serial(_serial, serialId, "serialId");
					serial_cast::AbstractWriter<S, ElementType> abstractWriter;
					abstractWriter.serial = &_serial;
					abstractWriter.value = _value.get();
					Serial<ElementType>::cast(abstractWriter, serialId);
					if (!abstractWriter.result.value())
					{
						return false;
					}
				}
			}
			else
			{
				// Read class
				bool exists = false;
				se_serial(_serial, exists, "exists");
				if (exists)
				{
					// NOTE: if there was a way to get this type from Serial<ElementType>::identify(), that would be great.
					using SerialId = typename SerialId<ElementType>::type;
					SerialId serialId;
					se_serial(_serial, serialId, "serialId");
					serial_cast::AbstractReader<S, ElementType> abstractReader;
					abstractReader.serial = &_serial;
					Serial<ElementType>::template cast(abstractReader, serialId);
					_value.swap(abstractReader.ptr);
					if (!_value)
					{
						return false;
					}
				}
			}
		}
		else
		{
			if constexpr (S::getWritingEnabled())
			{
				// Write non-class
				const bool exists = _value.get() != nullptr;
				se_serial(_serial, exists, "exists");
				if (exists)
				{
					se_serial(_serial, *_value, "value");
				}
			}
			else
			{
				// Read non-class
				bool exists = false;
				se_serial(_serial, exists, "exists");
				{
					_value.reset(new ElementType());
					se_serial(_serial, *_value, "value");
				}
			}
		}
		return true;
	}
}
