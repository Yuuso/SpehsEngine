#include "stdafx.h"
#include "SpehsEngine/Graphics/Model.h"

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
				_modelNode.meshes.push_back(std::make_unique<Mesh>(_model, _modelNode, _meshData.meshes[meshIndex]->materialIndex, _meshData.meshes[meshIndex]->vertexBuffer, _meshData.meshes[meshIndex]->indexBuffer));
				_modelNode.meshes.back()->setName(_meshData.meshes[meshIndex]->name);
				_modelNode.meshes.back()->setPrimitiveType(_meshData.meshes[meshIndex]->type);
				while (_numMaterials <= _meshData.meshes[meshIndex]->materialIndex)
					_numMaterials++;
			}
			for (auto&& child : _meshDataNode.children)
			{
				_modelNode.children.push_back(std::make_unique<ModelNode>(_model, &_modelNode));
				processNode(_model, _meshData, *child.get(), *_modelNode.children.back().get(), _numMaterials);
			}
		};

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
			se_assert(numMaterialSlots > 0);
			se_assert(materials.size() <= numMaterialSlots);
			foreachPrimitive([](Primitive& _primitive) { enableBit(_primitive.updateFlags, PrimitiveUpdateFlag::TransformChanged); }); // Force transform update
			reloaded = true;
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
			foreachPrimitive([_state](Primitive& _primitive) { _primitive.setRenderState(_state); });
		}
		void Model::toggleRenderState()
		{
			foreachPrimitive([](Primitive& _primitive) { _primitive.toggleRenderState(); });
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
			foreachPrimitive([&](Primitive& _primitive) { _primitive.setColor(_color); });
		}

		void Model::setRenderFlags(const RenderFlagsType _renderFlags)
		{
			foreachPrimitive([_renderFlags](Primitive& _primitive) { _primitive.setRenderFlags(_renderFlags); });
		}
		void Model::enableRenderFlag(const RenderFlag _renderFlag)
		{
			foreachPrimitive([_renderFlag](Primitive& _primitive) { _primitive.enableRenderFlag(_renderFlag); });
		}
		void Model::disableRenderFlag(const RenderFlag _renderFlag)
		{
			foreachPrimitive([_renderFlag](Primitive& _primitive) { _primitive.disableRenderFlag(_renderFlag); });
		}
		void Model::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			foreachPrimitive([_primitiveType](Primitive& _primitive) { _primitive.setPrimitiveType(_primitiveType); });
		}
		void Model::setRenderMode(const RenderMode _renderMode)
		{
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
