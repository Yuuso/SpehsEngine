#include "SpehsEngine/Sync/SyncTypeInfo.h"
#include "SpehsEngine/Net/Protocol.h"



namespace spehs
{
	namespace sync
	{
		void LocalSyncTypeInfo::write(net::WriteBuffer& buffer) const
		{
			buffer.write(name);
			buffer.write(typeId);
			buffer.write(version);
		}

		void LocalSyncTypeInfo::read(net::ReadBuffer& buffer)
		{
			buffer.read(name);
			buffer.read(typeId);
			buffer.read(version);
		}

		void RemoteSyncTypeInfo::write(net::WriteBuffer& buffer) const
		{
			buffer.write(name);
			buffer.write(typeId);
			buffer.write(version);
		}

		void RemoteSyncTypeInfo::read(net::ReadBuffer& buffer)
		{
			buffer.read(name);
			buffer.read(typeId);
			buffer.read(version);
		}

		void TypeInfo::write(net::WriteBuffer& buffer) const
		{
			buffer.write(local);
			buffer.write(remote);
		}

		void TypeInfo::read(net::ReadBuffer& buffer)
		{
			buffer.read(local);
			buffer.read(remote);
		}
	}
}