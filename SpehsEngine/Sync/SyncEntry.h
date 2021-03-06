#pragma once
#include <stdint.h>
#include <vector>
#include "SpehsEngine/Core/SE_Time.h"
#include "SyncTypeInfo.h"



namespace se
{
	namespace sync
	{
		class AbstractHandle;
		class Manager;
		class IType;
		
		/*
			Local instances are managed through entries.
		*/
		class Entry
		{
		public:
			typedef uint16_t Id;
		public:

			sync::Manager& manager;
			sync::IType* ptr;
			const Id id;
			const TypeInfo& typeInfo;
			const bool locallyInstantiated;//True if instantiated locally, lifetime depends on handles.

			bool createSent = false;
			bool removeSent = false;
			bool removeReceived = false;
			bool createReceived = false;

			bool sendRemoteUpdateInterval = false;
			se::time::Time requestedRemoteUpdateInterval = 0;

			std::vector<AbstractHandle*> handles;
			se::time::Time interval = 0;
			se::time::Time timer = 0;

		private:
			friend class Manager;
			Entry(Manager& _manager, IType* _passData, const Id _id, const TypeInfo& _typeInfo, const bool locallyInstantiated);
			virtual ~Entry();
		};
	}
}