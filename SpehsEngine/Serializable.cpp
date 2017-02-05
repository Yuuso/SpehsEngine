#include <atomic>
#include "Serializable.h"

namespace spehs
{
	std::atomic<int> serializableAllocations = 0;
	std::atomic<int> serializableDeallocations = 0;
	Serializable::Serializable()
	{
#ifdef SERIALIZABLE_DEBUG_INFO
		serializableAllocations++;
#endif
	}
	Serializable::Serializable(const spehs::Serializable& copy)
	{
#ifdef SERIALIZABLE_DEBUG_INFO
		serializableAllocations++;
#endif
	}
	Serializable::Serializable(spehs::Serializable&& move)
	{
#ifdef SERIALIZABLE_DEBUG_INFO
		serializableAllocations++;
#endif
	}
	Serializable::~Serializable()
	{
#ifdef SERIALIZABLE_DEBUG_INFO
		serializableDeallocations++;
#endif
	}
}