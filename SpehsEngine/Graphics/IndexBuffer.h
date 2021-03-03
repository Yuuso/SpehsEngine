#pragma once

#include "SpehsEngine/Graphics/Internal/BufferObject.h"
#include "SpehsEngine/Graphics/Internal/ResourceHandle.h"
#include "SpehsEngine/Graphics/Types.h"


namespace se
{
	namespace graphics
	{
		class IndexBuffer : public BufferObject
		{
		public:

			IndexBuffer();
			~IndexBuffer();

			IndexBuffer(const IndexBuffer& _other);
			IndexBuffer& operator=(const IndexBuffer& _other);

			IndexBuffer(IndexBuffer&& _other);
			IndexBuffer& operator=(IndexBuffer&& _other);


			void updateBuffer() const override;

			inline IndexType& operator[](const size_t _index) { return buffer[_index]; }
			inline const IndexType& operator[](const size_t _index) const { return buffer[_index]; }
			inline void resize(const size_t _size) { buffer.resize(_size); }
			inline void pushBack(const IndexBuffer& _other) { buffer.insert(buffer.end(), _other.buffer.begin(), _other.buffer.end()); }
			inline void pushBack(const IndexType _index) { buffer.push_back(_index); }
			inline void erase(const size_t _begin, const size_t _end) { buffer.erase(buffer.begin() + _begin, buffer.begin() + _end); }
			inline void clear() { buffer.clear(); }
			inline size_t size() const { return buffer.size(); }
			inline size_t bytes() const { return buffer.size() * sizeof(IndexType); }
			inline const void* data() const { return reinterpret_cast<const void*>(buffer.data()); }

		private:

			std::vector<IndexType> buffer;
		};
	}
}
