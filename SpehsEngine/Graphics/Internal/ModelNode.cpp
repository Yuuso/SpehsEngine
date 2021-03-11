#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/ModelNode.h"

#include "SpehsEngine/Graphics/Model.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"


namespace se
{
	namespace graphics
	{
		ModelNode::ModelNode(const class Model& _model, const ModelNode* _parent)
			: model(_model)
			, parent(_parent)
		{
		}

		void ModelNode::reset()
		{
			meshes.clear();
			children.clear();
			name.clear();
			transform = glm::mat4(1.0f);
		}

		glm::mat4 ModelNode::getTransform() const
		{
			if (parent)
				return parent->getTransform() * getLocalTransform();
			return glm::translate(model.getPosition()) * glm::mat4_cast(model.getRotation()) * glm::scale(model.getScale()) * getLocalTransform();
		}

		glm::mat4 ModelNode::getLocalTransform() const
		{
			if (model.activeAnimation)
			{
				auto it = model.activeAnimation->channels.find(name);
				if (it == model.activeAnimation->channels.end())
					return transform;

				const float timeInSeconds = time::getRunTime().asSeconds();
				const float timeInFrames = timeInSeconds * model.activeAnimation->framesPerSeconds;
				const float animTime = static_cast<float>(fmod(timeInFrames, model.activeAnimation->numFrames));
				return makeTransform(it->second, animTime);
			}
			else
			{
				return transform;
			}
		}

		void ModelNode::foreachMesh(std::function<void(Primitive&)> _fn)
		{
			for (auto&& mesh : meshes)
			{
				_fn(*mesh.get());
			}
			for (auto&& child : children)
			{
				child->foreachMesh(_fn);
			}
		}
		const Mesh* ModelNode::getMesh(const std::string_view _name) const
		{
			for (auto&& mesh : meshes)
			{
				if (mesh->getName() == _name)
					return mesh.get();
			}
			for (auto&& child : children)
			{
				const Mesh* result = child->getMesh(_name);
				if (result)
					return result;
			}
			return nullptr;
		}
		const Mesh* ModelNode::getMesh(size_t& _counter) const
		{
			for (auto&& mesh : meshes)
			{
				if (_counter == 0)
					return mesh.get();
				_counter--;
			}
			for (auto&& child : children)
			{
				const Mesh* result = child->getMesh(_counter);
				if (result)
					return result;
			}
			return nullptr;
		}
		Mesh* ModelNode::getMesh(const std::string_view _name)
		{
			for (auto&& mesh : meshes)
			{
				if (mesh->getName() == _name)
					return mesh.get();
			}
			for (auto&& child : children)
			{
				Mesh* result = child->getMesh(_name);
				if (result)
					return result;
			}
			return nullptr;
		}
		Mesh* ModelNode::getMesh(size_t& _counter)
		{
			for (auto&& mesh : meshes)
			{
				if (_counter == 0)
					return mesh.get();
				_counter--;
			}
			for (auto&& child : children)
			{
				Mesh* result = child->getMesh(_counter);
				if (result)
					return result;
			}
			return nullptr;
		}
	}
}
