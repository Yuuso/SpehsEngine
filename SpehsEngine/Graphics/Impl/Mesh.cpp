#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/Mesh.h"

#include "SpehsEngine/Graphics/Impl/ModelNode.h"
#include "SpehsEngine/Graphics/Model.h"


Mesh::Mesh(Model& _model, const ModelNode& _node, const size_t _materialIndex, std::shared_ptr<VertexBuffer> _vertexBuffer, std::shared_ptr<IndexBuffer> _indexBuffer)
	: model(_model)
	, node(_node)
	, materialIndex(_materialIndex)
{
	setVertices(_vertexBuffer);
	setIndices(_indexBuffer);

	combinedTransformMatrices.resize(1);
	combinedNormalMatrices.resize(1);
}

void Mesh::update()
{
	Primitive::update();
}
void Mesh::updateMatrices()
{
	if (bones.empty())
	{
		Primitive::updateMatrices();
		combinedTransformMatrices[0] = Primitive::getTransformMatrices()[0] * node.getTransform();
		combinedNormalMatrices[0] = glm::mat4(glm::inverse(glm::transpose(glm::mat3(combinedTransformMatrices[0]))));
	}
	else
	{
		for (size_t boneIndex = 0; boneIndex < bones.size(); boneIndex++)
		{
			combinedTransformMatrices[boneIndex] = (Primitive::getTransformMatrices()[0] * bones[boneIndex].boneNode->getTransform()) * bones[boneIndex].offsetMatrix;
			combinedNormalMatrices[boneIndex] = glm::mat4(glm::inverse(glm::transpose(glm::mat3(combinedTransformMatrices[boneIndex]))));
		}
	}
}
void Mesh::clearOverriddenModelAttributes()
{
	overriddenModelAttributes = 0;
}

void Mesh::addBone(const ModelNode* _boneNode, const glm::mat4& _offsetMatrix)
{
	se_assert(_boneNode);
	bones.emplace_back(_boneNode, _offsetMatrix);
	combinedTransformMatrices.resize(bones.size());
	combinedNormalMatrices.resize(bones.size());
}

void Mesh::setRenderState(bool _state)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderState);
	Primitive::setRenderState(_state);
}
void Mesh::toggleRenderState()
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderState);
	Primitive::toggleRenderState();
}
void Mesh::setMaterial(std::shared_ptr<Material> _material)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::Material);
	Primitive::setMaterial(_material);
}
void Mesh::setColor(const Color& _color)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::Color);
	Primitive::setColor(_color);
}
void Mesh::setRenderFlags(RenderFlagsType _renderFlags)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderFlags);
	Primitive::setRenderFlags(_renderFlags);
}
void Mesh::enableRenderFlags(RenderFlagsType _renderFlags)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderFlags);
	Primitive::enableRenderFlags(_renderFlags);
}
void Mesh::disableRenderFlags(RenderFlagsType _renderFlags)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderFlags);
	Primitive::disableRenderFlags(_renderFlags);
}
void Mesh::setPrimitiveType(PrimitiveType _primitiveType)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::PrimitiveType);
	Primitive::setPrimitiveType(_primitiveType);
}
void Mesh::setRenderMode(RenderMode _renderMode)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderMode);
	Primitive::setRenderMode(_renderMode);
}
void Mesh::setInstances(std::shared_ptr<VertexBuffer> _instances)
{
	enableBit(overriddenModelAttributes, PrimitiveAttributeFlag::Instances);
	Primitive::setInstances(_instances);
}

bool Mesh::getRenderState() const
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderState))
		return Primitive::getRenderState() && model.getRenderState(); // NOTE: Model value still stands when false
	return model.getRenderState();
}
std::shared_ptr<const Material>	Mesh::getMaterial() const
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::Material))
		return Primitive::getMaterial();
	return model.getMaterial(materialIndex);
}
std::shared_ptr<Material> Mesh::getMaterial()
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::Material))
		return Primitive::getMaterial();
	return model.getMaterial(materialIndex);
}
std::shared_ptr<const VertexBuffer> Mesh::getInstances() const
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::Instances))
		return Primitive::getInstances();
	return model.getInstances();
}
std::shared_ptr<VertexBuffer> Mesh::getInstances()
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::Instances))
		return Primitive::getInstances();
	return model.getInstances();
}
const Color& Mesh::getColor() const
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::Color))
		return Primitive::getColor();
	return model.getColor();
}
RenderFlagsType Mesh::getRenderFlags() const
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderFlags))
		return Primitive::getRenderFlags();
	return model.getRenderFlags();
}
PrimitiveType Mesh::getPrimitiveType() const
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::PrimitiveType) ||
		model.getPrimitiveType() == PrimitiveType::Undefined)
		return Primitive::getPrimitiveType();
	return model.getPrimitiveType();
}
RenderMode Mesh::getRenderMode() const
{
	if (checkBit(overriddenModelAttributes, PrimitiveAttributeFlag::RenderMode))
		return Primitive::getRenderMode();
	return model.getRenderMode();
}

const UniformMatrices& Mesh::getTransformMatrices() const
{
	return combinedTransformMatrices;
}
const UniformMatrices& Mesh::getNormalMatrices() const
{
	return combinedNormalMatrices;
}
