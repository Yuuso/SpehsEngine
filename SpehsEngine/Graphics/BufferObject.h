#pragma once

#include "SpehsEngine/Core/Asset.h"

namespace se::gfx::impl
{
	class Batch;
	class PrimitiveInternal;
}

namespace se::gfx
{
	class BufferObject
	{
	public:

		BufferObject();
		virtual ~BufferObject();

		BufferObject(const BufferObject& _other);
		BufferObject& operator=(const BufferObject& _other);

		BufferObject(BufferObject&& _other);
		BufferObject& operator=(BufferObject&& _other);


		AssetHandle getHandle() const;
		virtual void updateBuffer() = 0;

	protected:

		friend class impl::Batch;
		friend class impl::PrimitiveInternal;
		friend class Primitive;

		void registerAsRenderer(const uintptr_t _value);
		void unregisterAsRenderer(const uintptr_t _value);
		bool getBufferChanged() const { return bufferChanged; }

		AssetHandle bufferHandle;
		std::unordered_set<uintptr_t> renderers;
		bool bufferChanged = false;
	};
}
