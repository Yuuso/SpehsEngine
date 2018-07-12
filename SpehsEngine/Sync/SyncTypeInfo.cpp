#include "stdafx.h"
#include "SpehsEngine/Sync/SyncTypeInfo.h"
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/ReadBuffer.h>



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

		void LocalSyncTypeInfo::read(ReadBuffer& buffer)
		{
			buffer.read(name);
			buffer.read(typeId);
			buffer.read(version);
		}

		void RemoteSyncTypeInfo::write(WriteBuffer& buffer) const
		{
			buffer.write(name);
			buffer.write(typeId);
			buffer.write(version);
		}

		void RemoteSyncTypeInfo::read(ReadBuffer& buffer)
		{
			buffer.read(name);
			buffer.read(typeId);
			buffer.read(version);
		}

		void TypeInfo::write(WriteBuffer& buffer) const
		{
			buffer.write(local);
			buffer.write(remote);
		}

		void TypeInfo::read(ReadBuffer& buffer)
		{
			buffer.read(local);
			buffer.read(remote);
		}
	}
}