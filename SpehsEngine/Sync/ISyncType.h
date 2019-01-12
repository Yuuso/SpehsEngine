#pragma once
#include <stdint.h>
#include <string>
#include "SpehsEngine/Core/SE_Time.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	namespace sync
	{
		class IType
		{
		public:
			typedef uint32_t SyncTypeVersionType;
			typedef std::string SyncTypeIdType;
		public:

			virtual ~IType() {}

			/* Must implement the following static methods. Use macro below to do this the easy way. */
			//static const std::string& getSyncTypeName();
			//static const SyncTypeVersionType getSyncTypeVersion();

			/* By returning true, the sync type indicates that it is ready to write an update packet. */
			virtual bool syncUpdate(const se::time::Time deltaTime) { return false; }

			/* Sync packets */
			virtual void syncCreate(WriteBuffer& buffer) {}
			virtual void syncCreate(ReadBuffer& buffer) {}
			virtual void syncUpdate(WriteBuffer& buffer) {}
			virtual void syncUpdate(ReadBuffer& buffer) {}
			virtual void syncRemove(WriteBuffer& buffer) {}
			virtual void syncRemove(ReadBuffer& buffer) {}

		};
	}
}

#define SPEHS_SYNC_TYPE_DECL(p_Name, p_Version) \
public: \
static const std::string& getSyncTypeName() \
{ \
	static const std::string name(#p_Name); \
	return name; \
} \
static const SyncTypeIdType& getSyncTypeId() \
{ \
	static const SyncTypeIdType id = #p_Name; \
	return id; \
} \
static const SyncTypeVersionType getSyncTypeVersion() \
{ \
	static const SyncTypeVersionType version = p_Version; \
	return version; \
}