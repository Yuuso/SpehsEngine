#pragma once
#include <vector>
#include "SpehsEngine/Sync/ISyncType.h"
#include "SpehsEngine/Sync/SyncEntry.h"
#include "SpehsEngine/Core/Log.h"//SPEH_ASSERT...

namespace spehs
{
	namespace sync
	{
		class Manager;
		class Entry;

		class AbstractHandle
		{
		public:
			void setLocalUpdateInterval(const spehs::time::Time& interval);
			void setRemoteUpdateInterval(const spehs::time::Time& interval);
		protected:
			AbstractHandle(Entry* _entry);
			Entry* entry;
		};

		template<typename SyncType>
		class Handle : public AbstractHandle
		{
			friend class Manager;
		public:

			Handle()
				: AbstractHandle(nullptr)
			{

			}

			Handle(const Handle<SyncType>& other)
				: AbstractHandle(other.entry)
			{
				if (entry)
					entry->handles.push_back(this);
			}

			Handle(const Handle<SyncType>&& other) = delete;
			Handle(Handle<SyncType>&& other)
				: AbstractHandle(nullptr)
			{
				if (other.entry)
				{
					for (size_t i = 0; i < other.entry->handles.size(); i++)
					{
						if (other.entry->handles[i] == &other)
						{
							other.entry->handles[i] = this;
							entry = other.entry;
							other.entry = nullptr;
							break;
						}
					}
					SPEHS_ASSERT(other.entry == nullptr && entry != nullptr);
				}
			}

			~Handle()
			{
				free();
			}

			Handle<SyncType>& operator=(const Handle<SyncType>& other)
			{
				free();
				if (other.entry)
				{
					entry = other.entry;
					other.entry->handles.push_back(this);
				}
			}

			Handle<SyncType>& operator=(const Handle<SyncType>&& other)
			{
				free();
				if (other.entry)
				{
					bool found = false;
					for (size_t i = 0; i < other.entry->handles.size(); i++)
					{
						if (other.entry->handles[i] == &other)
						{
							other.entry->handles[i] = this;
							entry = other.entry;
							other.entry = nullptr;
							break;
						}
					}
					SPEHS_ASSERT(entry);
				}
			}

			/* Frees the referenced entry from this. The type remains to exist as long as other handles remain referencing it. */
			void free()
			{
				if (entry)
				{
					for (size_t i = 0; i < entry->handles.size(); i++)
					{
						if (entry->handles[i] == this)
						{
							entry->handles[i] = entry->handles.back();
							entry->handles.pop_back();
							entry = nullptr;
							break;
						}
					}
					SPEHS_ASSERT(entry == nullptr);
				}
			}

			/* A Handle is valid if its associated sync entry exists. */
			bool isValid() const
			{
				return entry != nullptr;
			}
			operator bool() const
			{
				return entry != nullptr;
			}

			operator SyncType*()
			{
				return entry ? ((SyncType*)entry->ptr) : nullptr;
			}
			operator const SyncType*() const
			{
				return entry ? ((SyncType*)entry->ptr) : nullptr;
			}

			SyncType* operator->()
			{
				return entry ? ((SyncType*)entry->ptr) : nullptr;
			}
			const SyncType* operator->() const
			{
				return entry ? ((SyncType*)entry->ptr) : nullptr;
			}

			SyncType& operator*()
			{
				return *((SyncType*)entry->ptr);
			}
			const SyncType& operator*() const
			{
				return *((SyncType*)entry->ptr);
			}

			SyncType* get()
			{
				return entry ? (SyncType*)entry->ptr : nullptr;
			}
			const SyncType* get() const
			{
				return entry ? (SyncType*)entry->ptr : nullptr;
			}

			sync::Manager* getManager()
			{
				return entry ? &entry->manager : nullptr;
			}
			const sync::Manager* getManager() const
			{
				return entry ? &entry->manager : nullptr;
			}
			
		private:
			Handle(sync::Entry& _entry)
				: AbstractHandle(&_entry)
			{
				entry->handles.push_back(this);
			}
		};
	}
}