#pragma once

#include "glm/gtc/quaternion.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "SpehsEngine/Graphics/Internal/BufferObject.h"
#include "SpehsEngine/Graphics/VertexAttribute.h"
#include <vector>


namespace se
{
	namespace graphics
	{
		struct InstanceData
		{
			glm::vec3 position = glm::vec3(0.0f);
			glm::quat rotation = glm::vec3(0.0f);
			float scale = 1.0f;
		};

		class InstanceBuffer : public BufferObject
		{
		public:

			InstanceBuffer();
			~InstanceBuffer();

			InstanceBuffer(const InstanceBuffer& _other);
			InstanceBuffer& operator=(const InstanceBuffer& _other);

			InstanceBuffer(InstanceBuffer&& _other);
			InstanceBuffer& operator=(InstanceBuffer&& _other);


			void updateBuffer() override;

			const size_t size() const;
			const size_t bytes() const;
			void resize(const size_t _size);
			void grow(const size_t _amount);
			void pushBack(const InstanceData& _data);
			void erase(const size_t _begin, const size_t _end);
			void clear();
			const void* data(const size_t _index = 0) const;

			void set(const size_t _at, const InstanceData& _data);
			const InstanceData get(const size_t _at) const;

		private:

			void setChangedAll();
			void clearChanged();

			size_t changedBegin = SIZE_MAX;
			size_t changedEnd = 0;
			std::vector<glm::mat4> buffer;
		};
	}
}
