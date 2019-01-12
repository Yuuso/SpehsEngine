#include "stdafx.h"
#include "SpehsEngine/Sync/SyncTypeInfo.h"

#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

namespace se
{
	namespace sync
	{
		void LocalSyncTypeInfo::write(WriteBuffer& buffer) const
		{
			buffer.write(name);
			buffer.write(typeId);
			buffer.write(version);
		}

		bool LocalSyncTypeInfo::read(ReadBuffer& buffer)
		{
			se_read(buffer, name);
			se_read(buffer, typeId);
			se_read(buffer, version);
			return true;
		}

		void RemoteSyncTypeInfo::write(WriteBuffer& buffer) const
		{
			buffer.write(name);
			buffer.write(typeId);
			buffer.write(version);
		}

		bool RemoteSyncTypeInfo::read(ReadBuffer& buffer)
		{
			se_read(buffer, name);
			se_read(buffer, typeId);
			se_read(buffer, version);
			return true;
		}

		void TypeInfo::write(WriteBuffer& buffer) const
		{
			buffer.write(local);
			buffer.write(remote);
		}

		bool TypeInfo::read(ReadBuffer& buffer)
		{
			se_read(buffer, local);
			se_read(buffer, remote);
			return true;
		}
	}
}