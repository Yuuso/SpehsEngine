#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/BufferObject.h"


namespace se
{
	namespace graphics
	{
		BufferObject::~BufferObject()
		{
			// TODO: Is this a problem?
			//se_assert(renderers.size() == 0);
		}

		BufferObject::BufferObject(const BufferObject& _other)
		{
			bufferChanged = true;
			// Don't change buffer object values
			// Only Derived data should be copied
		}
		BufferObject& BufferObject::operator=(const BufferObject& _other)
		{
			bufferChanged = true;
			// Don't change buffer object values
			// Only Derived data should be copied
			return *this;
		}

		BufferObject::BufferObject(BufferObject&& _other)
			: renderers(std::move(_other.renderers))
			, bufferObject(_other.bufferObject)
			, bufferChanged(_other.bufferChanged)
		{
			_other.bufferObject = INVALID_RESOURCE_HANDLE;
		}
		BufferObject& BufferObject::operator=(BufferObject&& _other)
		{
			renderers = std::move(_other.renderers);
			bufferObject = _other.bufferObject;
			bufferChanged = _other.bufferChanged;
			_other.bufferObject = INVALID_RESOURCE_HANDLE;
			return *this;
		}

		void BufferObject::registerAsRenderer(const uintptr_t _value)
		{
			const size_t sizeBefore = renderers.size();
			renderers.insert(_value);
			if (sizeBefore == 0 && renderers.size() > 0)
				updateBuffer();
		}
		void BufferObject::unregisterAsRenderer(const uintptr_t _value)
		{
			const size_t sizeBefore = renderers.size();
			renderers.erase(_value);
			if (sizeBefore > 0 && renderers.size() == 0)
				updateBuffer();
		}
	}
}
