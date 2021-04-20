#pragma once

#include "glm/gtc/quaternion.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "SpehsEngine/Graphics/Internal/BufferObject.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/VertexAttribute.h"
#include <vector>


namespace se
{
	namespace graphics
	{
		struct TransformInstanceData
		{
			glm::vec3 position = glm::vec3(0.0f);
			glm::quat rotation = glm::vec3(0.0f);
			float scale = 1.0f;
		};
		struct BillboardInstanceData
		{
			glm::vec3 position = glm::vec3(0.0f);
			float rotationAngle = 0.0f;
			glm::vec2 scale = glm::vec2(1.0f, 1.0f);
			glm::vec2 uvOffset = glm::vec2(0.0f, 0.0f);
		};


		class InstanceBuffer : public BufferObject
		{
		public:

			InstanceBuffer() = delete;
			InstanceBuffer(const InstanceBufferType _type);
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
			void pushBack(const TransformInstanceData& _data);
			void pushBack(const BillboardInstanceData& _data);
			void erase(const size_t _begin, const size_t _end);
			void erase(const size_t _at) { erase(_at, _at); }
			void clear();
			const void* data(const size_t _index = 0) const;

			void set(const size_t _at, const TransformInstanceData& _data);
			void set(const size_t _at, const BillboardInstanceData& _data);
			const TransformInstanceData getTransformData(const size_t _at) const;
			const BillboardInstanceData getBillboardData(const size_t _at) const;

		private:

			void setChangedAll();
			void clearChanged();
			size_t instanceBytes() const;

			const InstanceBufferType type;
			size_t changedBytesBegin = SIZE_MAX;
			size_t changedBytesEnd = 0;
			std::vector<uint8_t> buffer;
		};
	}
}
