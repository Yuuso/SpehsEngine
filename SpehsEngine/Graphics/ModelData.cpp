#include "stdafx.h"
#include "SpehsEngine/Graphics/ModelData.h"

#include <assimp/config.h>
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


namespace se
{
	namespace graphics
	{
		ModelData::ModelData(const std::string_view _name)
			: Resource(_name)
		{
		}
		ModelData::~ModelData()
		{
			destroy();
		}

		void ModelData::reload(std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			if (!reloadable())
			{
				log::error("Cannot reload model data!");
				return;
			}
			destroy();
			create(path, _resourceLoader);
		}
		bool ModelData::reloadable() const
		{
			return !path.empty();
		}

		static bool processNode(MeshData::ModelNode& _modelNode, const aiNode& _node)
		{
			_modelNode.name = _node.mName.C_Str();
			_modelNode.meshIndices.resize(_node.mNumMeshes);
			for (size_t i = 0; i < _node.mNumMeshes; i++)
				_modelNode.meshIndices[i] = _node.mMeshes[i];

			_modelNode.transform[0][0] = _node.mTransformation.a1; _modelNode.transform[0][1] = _node.mTransformation.b1;  _modelNode.transform[0][2] = _node.mTransformation.c1; _modelNode.transform[0][3] = _node.mTransformation.d1;
			_modelNode.transform[1][0] = _node.mTransformation.a2; _modelNode.transform[1][1] = _node.mTransformation.b2;  _modelNode.transform[1][2] = _node.mTransformation.c2; _modelNode.transform[1][3] = _node.mTransformation.d2;
			_modelNode.transform[2][0] = _node.mTransformation.a3; _modelNode.transform[2][1] = _node.mTransformation.b3;  _modelNode.transform[2][2] = _node.mTransformation.c3; _modelNode.transform[2][3] = _node.mTransformation.d3;
			_modelNode.transform[3][0] = _node.mTransformation.a4; _modelNode.transform[3][1] = _node.mTransformation.b4;  _modelNode.transform[3][2] = _node.mTransformation.c4; _modelNode.transform[3][3] = _node.mTransformation.d4;

			_modelNode.children.resize(_node.mNumChildren);
			for (unsigned int i = 0; i < _node.mNumChildren; i++)
			{
				_modelNode.children[i] = std::make_unique<MeshData::ModelNode>();
				if (!processNode(*_modelNode.children[i].get(), *_node.mChildren[i]))
					return false;
			}
			return true;
		}
		std::shared_ptr<ResourceData> ModelData::createResource(const std::string _path)
		{
			Assimp::Importer importer;
			constexpr int maxVertices = UINT16_MAX;
			constexpr int maxIndices = UINT16_MAX;
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, maxVertices);
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, maxIndices / 3);
			const aiScene* scene = importer.ReadFile(_path, aiProcess_CalcTangentSpace
														  | aiProcess_JoinIdenticalVertices
														  | aiProcess_Triangulate
														  | aiProcess_GenSmoothNormals
														  | aiProcess_SplitLargeMeshes
														  | aiProcess_LimitBoneWeights
														  | aiProcess_ValidateDataStructure
														  | aiProcess_PopulateArmatureData
														  | aiProcess_SortByPType
														  | aiProcess_FindDegenerates
														  | aiProcess_FindInvalidData
														  | aiProcess_GenUVCoords
														  | aiProcess_OptimizeMeshes
														  | aiProcess_OptimizeGraph
														  | aiProcess_SplitByBoneCount);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				log::error("Error loading model: " + _path + " | Assimp error: " + importer.GetErrorString());
				return nullptr;
			}

			if (scene->mNumTextures > 0)
				log::error(_path + ": Embedded textures not supported!");
			if (scene->mNumLights > 0)
				log::error(_path + ": Scene lights not supported!");
			if (scene->mNumCameras > 0)
				log::error(_path + ": Scene cameras not supported!");

			std::shared_ptr<MeshData> resource = std::make_shared<MeshData>();

			resource->meshes.resize(scene->mNumMeshes);
			for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
			{
				const aiMesh& sceneMesh = *scene->mMeshes[meshIndex];
				MeshData::MeshInfo& mesh = *resource->meshes[meshIndex].get();

				switch (sceneMesh.mPrimitiveTypes)
				{
					case aiPrimitiveType::aiPrimitiveType_POINT:	mesh.type = PrimitiveType::Points;		break;
					case aiPrimitiveType::aiPrimitiveType_LINE:		mesh.type = PrimitiveType::Lines;		break;
					case aiPrimitiveType::aiPrimitiveType_TRIANGLE:	mesh.type = PrimitiveType::Triangles;	break;
					default:
						log::error("Error loading model: " + _path + ", unknown primitive type!");
						return nullptr;
				}

				mesh.materialIndex = sceneMesh.mMaterialIndex;

				using namespace VertexAttribute;
				VertexAttributeFlagsType attributes = Position;
				se_assert(sceneMesh.HasPositions());
				se_assert(sceneMesh.HasFaces());
				if (sceneMesh.HasNormals())
					enableBit(attributes, Normal);
				if (sceneMesh.HasTangentsAndBitangents())
					enableBit(attributes, Tangent | Bitangent);
				if (sceneMesh.HasVertexColors(0))
					enableBit(attributes, Color0);
				if (sceneMesh.HasVertexColors(1))
					enableBit(attributes, Color1);
				if (sceneMesh.HasVertexColors(2))
					enableBit(attributes, Color2);
				if (sceneMesh.HasVertexColors(3))
					enableBit(attributes, Color3);
				if (sceneMesh.HasTextureCoords(0))
					enableBit(attributes, TexCoord0);
				if (sceneMesh.HasTextureCoords(1))
					enableBit(attributes, TexCoord1);
				if (sceneMesh.HasTextureCoords(2))
					enableBit(attributes, TexCoord2);
				se_assert_m(sceneMesh.GetNumUVChannels() <= 3, "Max 3 UV channels are currently supported!");
				if (sceneMesh.HasBones())
					enableBit(attributes, Weight | Indices);

				mesh.vertexBuffer = std::make_shared<VertexBuffer>(attributes);
				mesh.vertexBuffer->resize(sceneMesh.mNumVertices);
				se_assert(mesh.vertexBuffer->size() <= maxVertices);
				mesh.indexBuffer = std::make_shared<IndexBuffer>();
				mesh.indexBuffer->resize(sceneMesh.mNumFaces * 3);
				se_assert(mesh.indexBuffer->size() <= maxIndices);

				for (size_t vertexIndex = 0; vertexIndex < sceneMesh.mNumVertices; vertexIndex++)
				{
					mesh.vertexBuffer->get<Position>(vertexIndex) = glm::vec3(sceneMesh.mVertices[vertexIndex].x, sceneMesh.mVertices[vertexIndex].y, sceneMesh.mVertices[vertexIndex].z);
					if (checkBit(attributes, Normal))
						mesh.vertexBuffer->get<Normal>(vertexIndex) = glm::vec3(sceneMesh.mNormals[vertexIndex].x, sceneMesh.mNormals[vertexIndex].y, sceneMesh.mNormals[vertexIndex].z);
					if (checkBit(attributes, Tangent))
						mesh.vertexBuffer->get<Tangent>(vertexIndex) = glm::vec3(sceneMesh.mTangents[vertexIndex].x, sceneMesh.mTangents[vertexIndex].y, sceneMesh.mTangents[vertexIndex].z);
					if (checkBit(attributes, Bitangent))
						mesh.vertexBuffer->get<Bitangent>(vertexIndex) = glm::vec3(sceneMesh.mBitangents[vertexIndex].x, sceneMesh.mBitangents[vertexIndex].y, sceneMesh.mBitangents[vertexIndex].z);
					if (checkBit(attributes, Color0))
						mesh.vertexBuffer->get<Color0>(vertexIndex) = Color(sceneMesh.mColors[0][vertexIndex].r, sceneMesh.mColors[0][vertexIndex].g, sceneMesh.mColors[0][vertexIndex].b, sceneMesh.mColors[0][vertexIndex].a);
					if (checkBit(attributes, Color1))
						mesh.vertexBuffer->get<Color1>(vertexIndex) = Color(sceneMesh.mColors[1][vertexIndex].r, sceneMesh.mColors[1][vertexIndex].g, sceneMesh.mColors[1][vertexIndex].b, sceneMesh.mColors[1][vertexIndex].a);
					if (checkBit(attributes, Color2))
						mesh.vertexBuffer->get<Color2>(vertexIndex) = Color(sceneMesh.mColors[2][vertexIndex].r, sceneMesh.mColors[2][vertexIndex].g, sceneMesh.mColors[2][vertexIndex].b, sceneMesh.mColors[2][vertexIndex].a);
					if (checkBit(attributes, Color3))
						mesh.vertexBuffer->get<Color3>(vertexIndex) = Color(sceneMesh.mColors[3][vertexIndex].r, sceneMesh.mColors[3][vertexIndex].g, sceneMesh.mColors[3][vertexIndex].b, sceneMesh.mColors[3][vertexIndex].a);
					if (checkBit(attributes, TexCoord0))
						mesh.vertexBuffer->get<TexCoord0>(vertexIndex) = glm::vec2(sceneMesh.mTextureCoords[0][vertexIndex].x, sceneMesh.mTextureCoords[0][vertexIndex].y);
					if (checkBit(attributes, TexCoord1))
						mesh.vertexBuffer->get<TexCoord1>(vertexIndex) = glm::vec2(sceneMesh.mTextureCoords[1][vertexIndex].x, sceneMesh.mTextureCoords[1][vertexIndex].y);
					if (checkBit(attributes, TexCoord2))
						mesh.vertexBuffer->get<TexCoord2>(vertexIndex) = glm::vec2(sceneMesh.mTextureCoords[2][vertexIndex].x, sceneMesh.mTextureCoords[2][vertexIndex].y);
					// TODO: bone weights and bone indices
				}
				for (size_t faceIndex = 0; faceIndex < sceneMesh.mNumFaces; faceIndex++)
				{
					const aiFace& face = sceneMesh.mFaces[faceIndex];
					for (unsigned int indexIndex = 0; indexIndex < face.mNumIndices; indexIndex++)
					{
						const unsigned int index = face.mIndices[indexIndex];
						se_assert(index < std::numeric_limits<IndexType>::max());
						mesh.indexBuffer->at(faceIndex + indexIndex) = static_cast<IndexType>(index);
					}
				}
			}

			const bool result = processNode(resource->rootNode, *scene->mRootNode);
			if (!result)
			{
				log::error("Error processing model: " + _path);
				return nullptr;
			}
			return resource;
		}
		void ModelData::destroy()
		{
			resourceData.reset();
		}
		void ModelData::create(const std::string_view _path, std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			path = _path;
			if (_resourceLoader)
			{
				std::function<std::shared_ptr<ResourceData>()> func = std::bind(&ModelData::createResource, path);
				resourceFuture = _resourceLoader->push(func);
			}
			else
			{
				resourceData = std::dynamic_pointer_cast<MeshData>(createResource(path));
			}
		}
	}
}
