#pragma once

#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "SpehsEngine/Graphics/VertexAttribute.h"


namespace se::gfx
{
	struct TransformInstanceData
	{
		glm::vec3 getPosition() const;
		glm::quat getRotation() const;
		glm::vec3 getScale() const;

		void setPosition(const glm::vec3& _position);
		void setRotation(const glm::quat& _rotation);
		void setScale(const glm::vec3& _scale);

		void construct(const glm::vec3& _position, const glm::quat& _rotation, const glm::vec3& _scale);
		void deconstruct(glm::vec3* _position, glm::quat* _rotation, glm::vec3* _scale) const;

		glm::mat4 transformMatrix = glm::mat4(1.0f);
	};

	struct BillboardInstanceData
	{
		glm::vec3 position		= glm::vec3(0.0f);
		float rotationAngle		= 0.0f;
		glm::vec2 scale			= glm::vec2(1.0f, 1.0f);
		glm::vec2 uvOffset		= glm::vec2(0.0f, 0.0f);
		//Color color			= Color(1.0f, 1.0f, 1.0f, 1.0f);
	};


	template<typename T>
	class InstanceBuffer
	{
		static constexpr size_t instanceDataMinSize = sizeof(VertexAttribute::Data0Type);
		static constexpr size_t maxInstanceDataStride = 5;
		static_assert((sizeof(T) % instanceDataMinSize) == 0, "Invalid instance data size!");
		static_assert(sizeof(T) <= (instanceDataMinSize * maxInstanceDataStride), "Invalid instance data size!");

	public:

		InstanceBuffer();
		InstanceBuffer(std::shared_ptr<VertexBuffer> _buffer);

		const T& get(const size_t _index) const;
		void set(const size_t _index, const T& _data);
		size_t pushBack(const T& _data);

		const size_t size() const;
		void resize(const size_t _size);
		void grow(const size_t _amount);
		void erase(const size_t _begin, const size_t _end);
		void erase(const size_t _index);
		void clear();

		std::shared_ptr<VertexBuffer> getBuffer() { return buffer; }

	private:

		VertexAttributeFlagsType getAttributes();
		std::shared_ptr<VertexBuffer> buffer;
	};



	template<typename T>
	InstanceBuffer<T>::InstanceBuffer()
	{
		buffer = std::make_shared<VertexBuffer>();
		buffer->setAttributes(getAttributes());
	}
	template<typename T>
	InstanceBuffer<T>::InstanceBuffer(std::shared_ptr<VertexBuffer> _buffer)
	{
		buffer = _buffer;
		se_assert(buffer);
		se_assert(getAttributes() == buffer->getAttributes());
	}

	template<typename T>
	VertexAttributeFlagsType InstanceBuffer<T>::getAttributes()
	{
		VertexAttributeFlagsType result = 0;
		for (size_t i = 0; i < maxInstanceDataStride; i++)
		{
			if (sizeof(T) >= instanceDataMinSize * (i + 1))
				enableBit(result, VertexAttribute::Data0 << i);
		}
		return result;
	}

	template<typename T>
	const T& InstanceBuffer<T>::get(const size_t _index) const
	{
		return *reinterpret_cast<const T*>(&buffer->get<VertexAttribute::Data0>(_index));
	}

	template<typename T>
	void InstanceBuffer<T>::set(const size_t _index, const T& _data)
	{
		if (buffer->size() <= _index)
		{
			log::error("InstanceBuffer::set Invalid index!");
			return;
		}
		void* dest = reinterpret_cast<void*>(&buffer->get<VertexAttribute::Data0>(_index));
		memcpy_s(dest, sizeof(T), reinterpret_cast<const void*>(&_data), sizeof(T));
	}

	template<typename T>
	size_t InstanceBuffer<T>::pushBack(const T& _data)
	{
		const size_t index = size();
		grow(1);
		set(index, _data);
		return index;
	}

	template<typename T>
	const size_t InstanceBuffer<T>::size() const
	{
		return buffer->size();
	}

	template<typename T>
	void InstanceBuffer<T>::resize(const size_t _size)
	{
		buffer->resize(_size);
	}

	template<typename T>
	void InstanceBuffer<T>::grow(const size_t _amount)
	{
		buffer->grow(_amount);
	}

	template<typename T>
	void InstanceBuffer<T>::erase(const size_t _begin, const size_t _end)
	{
		buffer->erase(_begin, _end);
	}

	template<typename T>
	void InstanceBuffer<T>::erase(const size_t _index)
	{
		buffer->erase(_index, _index);
	}

	template<typename T>
	void InstanceBuffer<T>::clear()
	{
		buffer->clear();
	}
}
