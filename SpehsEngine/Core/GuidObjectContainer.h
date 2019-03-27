#pragma once
#include "SpehsEngine/Core/Guid.h"
#include "SpehsEngine/Core/STLUnorderedMapUtilityFunctions.h"
#include <unordered_map>

namespace se
{
	/*
		Container class for dynamically allocated objects identified with a Guid.
	*/
	template<typename Object>
	class GuidObjectContainer
	{
	public:

		typedef std::unordered_map<Guid, std::unique_ptr<Object>, Guid::HashFunctor> UnderlyingContainerType;
		typedef typename UnderlyingContainerType::iterator Iterator;
		typedef typename UnderlyingContainerType::const_iterator ConstIterator;

		//Serialization
		template<typename SizeType = size_t>
		void write(WriteBuffer& writeBuffer) const
		{
			writeToBuffer<SizeType>(writeBuffer, objects);
		}
		template<typename SizeType = size_t>
		bool read(ReadBuffer& readBuffer)
		{
			return readFromBuffer<SizeType>(readBuffer, objects);
		}
		template<typename SizeType = size_t>
		Archive write() const
		{
			return writeToArchive<SizeType>(objects);
		}
		template<typename SizeType = size_t>
		bool read(const Archive& archive)
		{
			return readFromArchive<SizeType>(objects);
		}

		template<typename ObjectType = Object, typename Metadata, typename ... Args>
		Iterator allocate(const Guid::Flag _flags, const Metadata _metadata, Args ... args)
		{
			//Find a free index for this flag + metadata combination
			std::vector<uint32_t> takenIndices;
			for (Iterator it = begin(); it != end(); it++)
			{
				if (it->first.getFlags() == _flags && it->first.getMetadata<uint16_t>() == uint16_t(_metadata))
				{
					takenIndices.push_back(it->first.getIndex());
				}
			}
			std::sort(takenIndices.begin(), takenIndices.end());
			uint32_t freeIndex = takenIndices.empty() ? 0 : takenIndices.back() + 1;
			for (size_t i = 1; i < takenIndices.size(); i++)
			{
				if (takenIndices[i - 1] < takenIndices[i] - 1)
				{
					freeIndex = takenIndices[i - 1] + 1;
					break;
				}
			}

			const Guid guid(_flags, _metadata, freeIndex);
			if (find(guid))
			{
				se_assert(false && "Object allocation failed. Next index was taken.");
				return end();
			}
			else
			{
				const Iterator it = objects.insert_or_assign(guid, std::unique_ptr<Object>()).first;
				it->second.reset(new ObjectType(args...));
				return it;
			}
		}
		template<typename ObjectType = Object, typename ... Args>
		Iterator allocateWithGuid(const Guid guid, Args ... args)
		{
			if (find(guid))
			{
				//Index is taken
				return end();
			}
			else
			{
				//Index is free
				const Iterator it = objects.insert_or_assign(guid, std::unique_ptr<Object>()).first;
				it->second.reset(new ObjectType(args...));
				return it;
			}
		}
		void free(const Guid guid)
		{
			auto it = objects.find(guid);
			if (it != objects.end())
			{
				objects.erase(it);
			}
		}
		void clear()
		{
			objects.clear();
		}
		size_t size() const
		{
			return objects.size();
		}
		Object* find(const Guid guid)
		{
			auto it = objects.find(guid);
			if (it != objects.end())
			{
				return it->second.get();
			}
			else
			{
				return nullptr;
			}
		}
		const Object* find(const Guid guid) const
		{
			auto it = objects.find(guid);
			if (it != objects.end())
			{
				return it->second.get();
			}
			else
			{
				return nullptr;
			}
		}

		Iterator begin() { return objects.begin(); }
		ConstIterator begin() const { return objects.begin(); }
		Iterator end() { return objects.end(); }
		ConstIterator end() const { return objects.end(); }

	private:
		uint32_t nextObjectIndex = 0u;
		UnderlyingContainerType objects;
	};
}
