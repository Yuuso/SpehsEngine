#pragma once
#include <functional>
#include "SpehsEngine/Sync/ISyncType.h"



namespace spehs
{
	namespace sync
	{
		//struct AbstractSyncTypeInfo
		//{
		//	void write(net::WriteBuffer& buffer) const;
		//	void read(net::ReadBuffer& buffer);

		//	IType::SyncTypeIdType typeId;
		//	std::string name;
		//	IType::SyncTypeVersionType version;
		//};

		struct LocalSyncTypeInfo
		{
			void write(net::WriteBuffer& buffer) const;
			void read(net::ReadBuffer& buffer);
			std::string name;
			IType::SyncTypeIdType typeId;
			IType::SyncTypeVersionType version;
			std::function<IType*()> constructor;
		};

		struct RemoteSyncTypeInfo
		{
			void write(net::WriteBuffer& buffer) const;
			void read(net::ReadBuffer& buffer);
			std::string name;
			IType::SyncTypeIdType typeId;
			IType::SyncTypeVersionType version;
		};

		struct TypeInfo
		{
			void write(net::WriteBuffer& buffer) const;
			void read(net::ReadBuffer& buffer);
			LocalSyncTypeInfo local;
			RemoteSyncTypeInfo remote;
		};
	}
}