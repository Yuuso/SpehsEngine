#include "stdafx.h"
#include "SpehsEngine/Graphics/Model.h"

#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Graphics/ResourceData.h"


namespace se
{
	namespace graphics
	{
		Model::Model()
			: rootNode(*this, nullptr)
		{
		}
		Model::~Model()
		{
			destroyedSignal();
		}

		static void processNode(const Model& _model, const MeshData& _meshData, const MeshData::MeshDataNode& _meshDataNode, ModelNode& _modelNode, size_t& _numMaterials)
		{
			_modelNode.name = _meshDataNode.name;
			_modelNode.transform = _meshDataNode.transform;
			for (auto&& meshIndex : _meshDataNode.meshIndices)
			{
				_modelNode.meshes.push_back(std::make_unique<Mesh>(_model, _modelNode, _meshData.meshes[meshIndex].materialIndex, _meshData.meshes[meshIndex].vertexBuffer, _meshData.meshes[meshIndex].indexBuffer));
				_modelNode.meshes.back()->setName(_meshData.meshes[meshIndex].name);
				_modelNode.meshes.back()->setPrimitiveType(_meshData.meshes[meshIndex].type);
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

			rootNode.reset();
			numMaterialSlots = 0;

			if (!modelData->ready())
				return;

			const MeshData& meshData = *modelData->resourceData.get();
			processNode(*this, meshData, meshData.rootNode, rootNode, numMaterialSlots);
			addBones(meshData, rootNode, rootNode);
			globalInverseTransform = meshData.globalInverseMatrix;
			animations = meshData.animations;
			activeAnimation = nullptr;
			if (!activeAnimationName.empty())
				startAnimation(activeAnimationName);
			se_assert(numMaterialSlots > 0);
			se_assert(materials.size() <= numMaterialSlots);
			postReload();
			reloaded = true;
		}
		void Model::postReload()
		{
			setRenderState(renderState);
			setRenderFlags(renderFlags);
			setPrimitiveType(primitiveType);
			setRenderMode(renderMode);
			setColor(color);
			foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); }); // Force transform update
		}

		void Model::startAnimation(const std::string_view _name)
		{
			activeAnimationName = _name;
			if (!animations)
				return;
			// Find exact match
			for (size_t i = 0; i < animations->size(); i++)
			{
				if (animations->at(i).name == _name)
				{
					activeAnimation = &animations->at(i);
					return;
				}
			}
			// Settle for partial match...
			for (size_t i = 0; i < animations->size(); i++)
			{
				if (doesContain(animations->at(i).name, _name))
				{
					activeAnimation = &animations->at(i);
					return;
				}
			}
			log::warning("Animation " + _name + " not found in model " + name);
		}
		void Model::stopAnimation()
		{
			activeAnimation = nullptr;
			activeAnimationName.clear();
		}

		void Model::updateAnimation()
		{
			se_assert(activeAnimation);
			// Force update on all transforms
			foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); });
		}

		void Model::foreachPrimitive(std::function<void(Primitive&)> _fn)
		{
			rootNode.foreachMesh(_fn);
		}
		const Primitive* Model::getPrimitive(const std::string_view _meshName) const
		{
			return rootNode.getMesh(_meshName);
		}
		const Primitive* Model::getPrimitive(const size_t _index) const
		{
			size_t counter = _index;
			return rootNode.getMesh(counter);
		}
		Primitive* Model::getPrimitive(const std::string_view _meshName)
		{
			return rootNode.getMesh(_meshName);
		}
		Primitive* Model::getPrimitive(const size_t _index)
		{
			size_t counter = _index;
			return rootNode.getMesh(counter);
		}
		const size_t Model::getNumPrimitives() const
		{
			if (modelData)
				return modelData->resourceData->meshes.size();
			return 0;
		}


		const std::string& Model::getName() const
		{
			return name;
		}
		std::shared_ptr<Material> Model::getMaterial(const size_t _slot) const
		{
			se_assert(_slot < numMaterialSlots);
			if (_slot >= materials.size() || !materials[_slot])
				return materials[0];
			return materials[_slot];
		}
		const size_t Model::getNumMaterials() const
		{
			return materials.size();
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

		void Model::setName(const std::string_view _name)
		{
			name = _name;
		}
		void Model::setRenderState(const bool _state)
		{
			renderState = _state;
			foreachPrimitive([_state](Primitive& _primitive) { _primitive.setRenderState(_state); });
		}
		void Model::setMaterial(std::shared_ptr<Material> _material, const size_t _slot)
		{
			se_assert(numMaterialSlots == 0 || _slot < numMaterialSlots);
			if (_slot >= materials.size())
				materials.resize(_slot + 1);
			materials[_slot] = _material;
		}
		void Model::setColor(const Color& _color)
		{
			color = _color;
			foreachPrimitive([&](Primitive& _primitive) { _primitive.setColor(_color); });
		}

		void Model::setRenderFlags(const RenderFlagsType _renderFlags)
		{
			renderFlags = _renderFlags;
			foreachPrimitive([_renderFlags](Primitive& _primitive) { _primitive.setRenderFlags(_renderFlags); });
		}
		void Model::enableRenderFlags(const RenderFlagsType _renderFlags)
		{
			enableBit(renderFlags, _renderFlags);
			foreachPrimitive([_renderFlags](Primitive& _primitive) { _primitive.enableRenderFlags(_renderFlags); });
		}
		void Model::disableRenderFlags(const RenderFlagsType _renderFlags)
		{
			disableBit(renderFlags, _renderFlags);
			foreachPrimitive([_renderFlags](Primitive& _primitive) { _primitive.disableRenderFlags(_renderFlags); });
		}
		void Model::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			if (_primitiveType == PrimitiveType::Undefined)
				return;
			primitiveType = _primitiveType;
			foreachPrimitive([_primitiveType](Primitive& _primitive) { _primitive.setPrimitiveType(_primitiveType); });
		}
		void Model::setRenderMode(const RenderMode _renderMode)
		{
			renderMode = _renderMode;
			if (renderMode == RenderMode::Static && animations && !animations->empty())
				log::warning("Should not use static RenderMode with animated models!");
			foreachPrimitive([_renderMode](Primitive& _primitive) { _primitive.setRenderMode(_renderMode); });
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
	}
}
