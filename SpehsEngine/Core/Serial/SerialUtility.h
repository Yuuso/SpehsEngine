#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/Serial/ArchiveReader.h"
#include "SpehsEngine/Core/Serial/ArchiveWriter.h"
#include "SpehsEngine/Core/Serial/BinaryReader.h"
#include "SpehsEngine/Core/Serial/BinaryWriter.h"
#include <optional>


namespace se
{
	// The following templates are used for polymorphic serialization
	namespace serial_cast
	{
		// Serial<>::cast() target for writing of an abstract user class
		template<typename Serial, typename T>
		struct AbstractWriter
		{
			template<typename U>
			void cast()
			{
				if constexpr (Serial::getKeyEnabled())
				{
					result.emplace(serial->serial((const U&)(*value), "class"));
				}
				else
				{
					result.emplace(serial->serial((const U&)(*value)));
				}
			}
			Serial* serial = nullptr;
			const T* value = nullptr;
			std::optional<bool> result;
		};

		// Serial<>::cast() target for reading of an abstract user class
		template<typename Serial, typename T>
		struct AbstractReader
		{
			template<typename U>
			void cast()
			{
				U* const u = new U();
				ptr.reset(u);
				if constexpr (Serial::getKeyEnabled())
				{
					if (!serial->serial(*u, "class"))
					{
						ptr.reset();
					}
				}
				else
				{
					if (!serial->serial(*u))
					{
						ptr.reset();
					}
				}
			}
			Serial* serial = nullptr;
			std::unique_ptr<T> ptr;
		};
	}

	/*
		You can use this helper class to trigger template instantiation for the required templates used in polymorphic serialization.
		Useful if you want to hide the user class factory code in a cpp instead of a header.
		Usage: paste the following code in your cpp file and replace the UserType and UserSerialId types with your corresponding types.
			template struct se::InstantiateSerialCast<UserType, UserSerialId>;
	*/
	template<typename Class, typename SerialId>
	struct InstantiateSerialCast
	{
	private:
		// Private: no instance of this class is intended to be created.
		InstantiateSerialCast()
		{
			impl<serial_cast::AbstractReader<ArchiveReader, Class>>();
			impl<serial_cast::AbstractReader<BinaryReader, Class>>();
			impl<serial_cast::AbstractWriter<ArchiveWriter, Class>>();
			impl<serial_cast::AbstractWriter<BinaryWriter, Class>>();
		}
		template<typename CastTarget>
		void impl()
		{
			CastTarget castTarget;
			SerialId serialId;
			se::Serial<Class>::template cast<CastTarget, SerialId>(castTarget, serialId);
		}
	};
}
