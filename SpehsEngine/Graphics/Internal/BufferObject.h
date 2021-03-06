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


			virtual void updateBuffer() = 0;

		protected:

			friend class PrimitiveInstance;
			friend class Primitive;
			friend class Batch;

			void registerAsRenderer(const uintptr_t _value);
			void unregisterAsRenderer(const uintptr_t _value);
			bool getBufferChanged() const { return bufferChanged; }

			ResourceHandle bufferObject = INVALID_RESOURCE_HANDLE;
			std::set<uintptr_t> renderers;
			bool bufferChanged = false;
		};
	}
}
