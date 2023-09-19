#include "stdafx.h"
#include "SpehsEngine/Graphics/Model.h"

#include "SpehsEngine/Graphics/Impl/Animation.h"
#include "SpehsEngine/Graphics/Impl/AnimatorInternal.h"
#include "SpehsEngine/Graphics/Impl/Mesh.h"
#include "SpehsEngine/Graphics/Impl/ModelNode.h"
#include "SpehsEngine/Graphics/ModelData.h"
#include "SpehsEngine/Graphics/ResourceData.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"


Model::Model()
{
	rootNode = std::make_unique<ModelNode>(*this, nullptr);
	animator = std::make_unique<AnimatorInternal>();
}
Model::~Model()
{
	destroyedSignal();
}

static void processNode(Model& _model, const MeshData& _meshData, const MeshData::MeshDataNode& _meshDataNode, ModelNode& _modelNode, size_t& _numMaterials)
{
	_modelNode.name = _meshDataNode.name;
	_modelNode.transform = _meshDataNode.transform;
	for (auto&& meshIndex : _meshDataNode.meshIndices)
	{
		_modelNode.meshes.push_back(std::make_unique<Mesh>(_model, _modelNode, _meshData.meshes[meshIndex].materialIndex, _meshData.meshes[meshIndex].vertexBuffer, _meshData.meshes[meshIndex].indexBuffer));
		_modelNode.meshes.back()->setName(_meshData.meshes[meshIndex].name);
		_modelNode.meshes.back()->setPrimitiveType(_meshData.meshes[meshIndex].type);
		_modelNode.meshes.back()->clearOverriddenModelAttributes();
		while (_numMaterials <= _meshData.meshes[meshIndex].materialIndex)
			_numMaterials++;
	}
	for (auto&& child : _meshDataNode.children)
	{
		_modelNode.children.push_back(std::make_unique<ModelNode>(_model, &_modelNode));
		processNode(_model, _meshData, *child.get(), *_modelNode.children.back().get(), _numMaterials);
	}
};

static const ModelNode* getModelNode(ModelNode& _node, const std::string_view _name)
{
	if (_node.name == _name)
		return &_node;
	for (auto&& child : _node.children)
	{
		const ModelNode* result = getModelNode(*child.get(), _name);
		if (result)
			return result;
	}
	return nullptr;
}

static void addBones(const MeshData& _meshData, ModelNode& _rootNode, ModelNode& _modelNode)
{
	for (auto&& mesh : _modelNode.meshes)
	{
		for (auto&& meshData : _meshData.meshes)
		{
			if (meshData.name != mesh->getName())
				continue;
			for (auto&& bone : meshData.bones)
			{
				mesh->addBone(getModelNode(_rootNode, bone.boneNodeName), bone.offsetMatrix);
			}
		}
	}
	for (auto&& child : _modelNode.children)
	{
		addBones(_meshData, _rootNode, *child.get());
	}
}

void Model::loadModelData(std::shared_ptr<ModelData> _modelData)
{
	se_assert(_modelData);
	modelData = _modelData;
	modelDataLoadedConnection = modelData->resourceLoadedSignal.connect(boost::bind(&Model::reloadModeData, this));
	reloadModeData();
}
void Model::reloadModeData()
{
	if (!modelData)
	{
		log::warning("ModelData not set, cannot reload!");
		return;
	}

	rootNode->reset();
	numMaterialSlots = 0;

	if (!modelData->ready() || !modelData->resourceData)
		return;

	const MeshData& meshData = *modelData->resourceData.get();
	processNode(*this, meshData, meshData.rootNode, *rootNode, numMaterialSlots);
	addBones(meshData, *rootNode, *rootNode);
	globalInverseTransform = meshData.globalInverseMatrix;
	animator->setAnimations(meshData.animations);

	se_assert(numMaterialSlots > 0);
	se_assert(materials.size() <= numMaterialSlots);
	postReload();
	reloaded = true;
}
void Model::postReload()
{
	if (renderMode == RenderMode::Batched && animator->hasAnimations())
	{
		log::warning("Should not use RenderMode::Batched with animated models!");
	}
	setRenderState(renderState);
	setRenderFlags(renderFlags);
	setPrimitiveType(primitiveType);
	setRenderMode(renderMode);
	setColor(color);
	if (instances)
	{
		foreachPrimitive([this](Primitive& _primitive) { _primitive.setInstances(instances); });
	}
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); }); // Force transform update
}

Animator& Model::getAnimator()
{
	return *animator;
}
const Animator& Model::getAnimator() const
{
	return *animator;
}

void Model::updateAnimations()
{
	animator->update();
	if (animator->isPlaying())
	{
		// Force update on all transforms
		foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); });
	}
}

void Model::foreachPrimitive(std::function<void(Primitive&)> _fn)
{
	rootNode->foreachMesh(_fn);
}
void Model::foreachPrimitive(std::function<void(Primitive&, std::string_view)> _fn)
{
	rootNode->foreachMesh(_fn);
}
const Primitive* Model::getPrimitive(std::string_view _meshName) const
{
	return rootNode->getMesh(_meshName);
}
const Primitive* Model::getPrimitive(size_t _index) const
{
	size_t counter = _index;
	return rootNode->getMesh(counter);
}
Primitive* Model::getPrimitive(std::string_view _meshName)
{
	return rootNode->getMesh(_meshName);
}
Primitive* Model::getPrimitive(size_t _index)
{
	size_t counter = _index;
	return rootNode->getMesh(counter);
}
size_t Model::getNumPrimitives() const
{
	if (modelData)
		return modelData->resourceData->meshes.size();
	return 0;
}


const std::string& Model::getName() const
{
	return name;
}
bool Model::getRenderState() const
{
	return renderState;
}
std::shared_ptr<const Material> Model::getMaterial(size_t _slot) const
{
	se_assert(_slot < numMaterialSlots);
	if (_slot >= materials.size() || !materials[_slot])
		return materials.empty() ? nullptr : materials[0];
	return materials[_slot];
}
std::shared_ptr<Material> Model::getMaterial(size_t _slot)
{
	se_assert(_slot < numMaterialSlots);
	if (_slot >= materials.size() || !materials[_slot])
		return materials.empty() ? nullptr : materials[0];
	return materials[_slot];
}
size_t Model::getNumMaterials() const
{
	return materials.size();
}
const Color& Model::getColor() const
{
	return color;
}
RenderFlagsType Model::getRenderFlags() const
{
	return renderFlags;
}

std::shared_ptr<const VertexBuffer> Model::getInstances() const
{
	return instances;
}
std::shared_ptr<VertexBuffer> Model::getInstances()
{
	return instances;
}
PrimitiveType Model::getPrimitiveType() const
{
	return primitiveType;
}
RenderMode Model::getRenderMode() const
{
	return renderMode;
}
void Model::setInstances(std::shared_ptr<VertexBuffer> _instances)
{
	instances = _instances;
	if (instances && renderMode == RenderMode::Batched)
	{
		log::error("Should not use RenderMode::Batched with instanced primitives!");
	}
}

const glm::vec3& Model::getPosition() const
{
	return position;
}
const glm::quat& Model::getRotation() const
{
	return rotation;
}
const glm::vec3& Model::getScale() const
{
	return scale;
}

void Model::setName(std::string_view _name)
{
	name = _name;
}
void Model::setRenderState(bool _state)
{
	renderState = _state;
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::EverythingChanged); });
}
void Model::setMaterial(std::shared_ptr<Material> _material, size_t _slot)
{
	se_assert(numMaterialSlots == 0 || _slot < numMaterialSlots);
	if (_slot >= materials.size())
		materials.resize(_slot + 1);
	materials[_slot] = _material;
}
void Model::setColor(const Color& _color)
{
	color = _color;
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::RenderInfoChanged); });
}

void Model::setRenderFlags(RenderFlagsType _renderFlags)
{
	renderFlags = _renderFlags;
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::RenderInfoChanged); });
}
void Model::enableRenderFlags(RenderFlagsType _renderFlags)
{
	enableBit(renderFlags, _renderFlags);
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::RenderInfoChanged); });
}
void Model::disableRenderFlags(RenderFlagsType _renderFlags)
{
	disableBit(renderFlags, _renderFlags);
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::RenderInfoChanged); });
}
void Model::setPrimitiveType(PrimitiveType _primitiveType)
{
	if (_primitiveType == PrimitiveType::Undefined)
		return;
	primitiveType = _primitiveType;
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::RenderInfoChanged); });
}
void Model::setRenderMode(RenderMode _renderMode)
{
	renderMode = _renderMode;
	if (renderMode == RenderMode::Batched && animator->hasAnimations())
	{
		log::warning("Should not use RenderMode::Batched with animated models!");
	}
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::RenderInfoChanged); });
}

void Model::setPosition(const glm::vec3& _position)
{
	position = _position;
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); });
}
void Model::setRotation(const glm::quat& _rotation)
{
	rotation = _rotation;
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); });
}
void Model::setScale(const glm::vec3& _scale)
{
	scale = _scale;
	foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); });
}
