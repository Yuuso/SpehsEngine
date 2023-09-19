#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/ModelNode.h"

#include "SpehsEngine/Graphics/Impl/AnimatorInternal.h"
#include "SpehsEngine/Graphics/Impl/Mesh.h"
#include "SpehsEngine/Graphics/Model.h"


ModelNode::ModelNode(const class Model& _model, const ModelNode* _parent)
	: model(_model)
	, parent(_parent)
{}

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
	return constructTransformationMatrix(model.getPosition(), model.getRotation(), model.getScale()) * getLocalTransform();
}

glm::mat4 ModelNode::getLocalTransform() const
{
	if (model.animator->IsNodePlaying(name))
	{
		return model.animator->getTransform(name);
	}
	return transform;
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
void ModelNode::foreachMesh(std::function<void(Primitive&, std::string_view)> _fn)
{
	for (auto&& mesh : meshes)
	{
		_fn(*mesh.get(), name);
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
