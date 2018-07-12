#include "SpehsEngine/Sync/SyncHandle.h"
#include "SpehsEngine/Sync/SyncEntry.h"
#include "SpehsEngine/Sync/SyncManager.h"
#include "SpehsEngine/Net/SocketTCP.h"



namespace spehs
{
	namespace sync
	{
		AbstractHandle::AbstractHandle(Entry* _entry)
			: entry(_entry)
		{

		}

		void AbstractHandle::setLocalUpdateInterval(const time::Time& interval)
		{
			if (entry->interval != interval)
			{
				entry->interval = interval;
			}
		}

		void AbstractHandle::setRemoteUpdateInterval(const time::Time& interval)
		{
			entry->sendRemoteUpdateInterval = true;
			entry->requestedRemoteUpdateInterval = interval;
		}
	}
}