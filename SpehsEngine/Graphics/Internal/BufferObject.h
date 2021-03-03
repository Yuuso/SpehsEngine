#pragma once

#include "SpehsEngine/Graphics/Internal/ResourceHandle.h"
#include <set>
#include <stdint.h>


namespace se
{
	namespace graphics
	{
		class BufferObject
		{
		public:

			BufferObject() = default;
			virtual ~BufferObject();

			BufferObject(const BufferObject& _other);
			BufferObject& operator=(const BufferObject& _other);

			BufferObject(BufferObject&& _other);
			BufferObject& operator=(BufferObject&& _other);


			virtual void updateBuffer() const = 0;

		protected:

			friend class PrimitiveInstance;
			friend class Batch;
			void registerAsRenderer(const uintptr_t _value) const;
			void unregisterAsRenderer(const uintptr_t _value) const;
			mutable ResourceHandle bufferObject = INVALID_RESOURCE_HANDLE;
			mutable std::set<uintptr_t> renderers;
		};
	}
}
