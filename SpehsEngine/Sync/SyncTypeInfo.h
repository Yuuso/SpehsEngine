#pragma once
#include <functional>
#include "SpehsEngine/Sync/ISyncType.h"



namespace se
{
	namespace sync
	{
		//struct AbstractSyncTypeInfo
		//{
		//	void write(WriteBuffer& buffer) const;
		//	bool read(ReadBuffer& buffer);

		//	IType::SyncTypeIdType typeId;
		//	std::string name;
		//	IType::SyncTypeVersionType version;
		//};

		struct LocalSyncTypeInfo
		{
			void write(WriteBuffer& buffer) const;
			bool read(ReadBuffer& buffer);
			std::string name;
			IType::SyncTypeIdType typeId;
			IType::SyncTypeVersionType version;
			std::function<IType*()> constructor;
		};

		struct RemoteSyncTypeInfo
		{
			void write(WriteBuffer& buffer) const;
			bool read(ReadBuffer& buffer);
			std::string name;
			IType::SyncTypeIdType typeId;
			IType::SyncTypeVersionType version;
		};

		struct TypeInfo
		{
			void write(WriteBuffer& buffer) const;
			bool read(ReadBuffer& buffer);
			LocalSyncTypeInfo local;
			RemoteSyncTypeInfo remote;
		};
	}
}