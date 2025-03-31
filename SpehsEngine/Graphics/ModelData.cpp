#include "stdafx.h"
#include "SpehsEngine/Graphics/ModelData.h"

#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Graphics/Internal/Animation.h"
#include "SpehsEngine/Graphics/IndexBuffer.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "assimp/config.h"
#include "assimp/matrix4x4.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"


namespace se
{
	namespace graphics
	{
		constexpr int maxBoneWeights = 4;
		constexpr int maxBones = (int)SE_MAX_BONES; // Max bones per mesh, also defined by the shaders, note: aiProcess_SplitByBoneCount
		constexpr int maxVertices = UINT16_MAX;
		constexpr int maxIndices = UINT16_MAX;

		inline static glm::mat4 make_mat4(const aiMatrix4x4& _matrix)
		{
			return glm::transpose(glm::make_mat4(&_matrix.a1));
		}

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

		static bool processNode(MeshData::MeshDataNode& _modelNode, const aiNode& _node)
		{
			_modelNode.name = _node.mName.C_Str();
			_modelNode.meshIndices.resize(_node.mNumMeshes);
			for (size_t i = 0; i < _node.mNumMeshes; i++)
				_modelNode.meshIndices[i] = _node.mMeshes[i];

			_modelNode.transform = make_mat4(_node.mTransformation);

			_modelNode.children.resize(_node.mNumChildren);
			for (unsigned int i = 0; i < _node.mNumChildren; i++)
			{
				_modelNode.children[i] = std::make_unique<MeshData::MeshDataNode>();
				if (!processNode(*_modelNode.children[i].get(), *_node.mChildren[i]))
					return false;
			}
			return true;
		}

		static void createResourcePrintMetadata(const std::string _path, const aiMetadata& _metadata)
		{
			for (size_t metaDataIndex = 0; metaDataIndex < _metadata.mNumProperties; metaDataIndex++)
			{
				switch (_metadata.mValues[metaDataIndex].mType)
				{
				case AI_BOOL:
				{
					const bool value = *reinterpret_cast<bool*>(_metadata.mValues[metaDataIndex].mData);
					log::info(_path + ", metadata: " + _metadata.mKeys[metaDataIndex].C_Str() + " = " + (value ? "true" : "false"));
				} break;
				case AI_INT32:
				{
					const int32_t value = *reinterpret_cast<int32_t*>(_metadata.mValues[metaDataIndex].mData);
					log::info(_path + ", metadata: " + _metadata.mKeys[metaDataIndex].C_Str() + " = " + std::to_string(value));
				} break;
				case AI_UINT64:
				{
					const uint64_t value = *reinterpret_cast<uint64_t*>(_metadata.mValues[metaDataIndex].mData);
					log::info(_path + ", metadata: " + _metadata.mKeys[metaDataIndex].C_Str() + " = " + std::to_string(value));
				} break;
				case AI_FLOAT:
				{
					const float value = *reinterpret_cast<float*>(_metadata.mValues[metaDataIndex].mData);
					log::info(_path + ", metadata: " + _metadata.mKeys[metaDataIndex].C_Str() + " = " + std::to_string(value));
				} break;
				case AI_DOUBLE:
				{
					const double value = *reinterpret_cast<double*>(_metadata.mValues[metaDataIndex].mData);
					log::info(_path + ", metadata: " + _metadata.mKeys[metaDataIndex].C_Str() + " = " + std::to_string(value));
				} break;
				case AI_AISTRING:
				{
					const aiString value = *reinterpret_cast<aiString*>(_metadata.mValues[metaDataIndex].mData);
					log::info(_path + ", metadata: " + _metadata.mKeys[metaDataIndex].C_Str() + " = " + value.C_Str());
				} break;
				case AI_AIVECTOR3D:
				{
					const aiVector3D value = *reinterpret_cast<aiVector3D*>(_metadata.mValues[metaDataIndex].mData);
					log::info(_path + ", metadata: " + _metadata.mKeys[metaDataIndex].C_Str() + " = " + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z));
				} break;
				default: break;
				}
			}
		}

		static constexpr glm::quat toQuat(const aiQuaternion& _aiQuaternion)
		{
			return glm::quat(_aiQuaternion.w, _aiQuaternion.x, _aiQuaternion.y, _aiQuaternion.z);
		}

		static constexpr glm::vec3 toVec3(const aiVector3D& _aiVector)
		{
			return glm::vec3(_aiVector.x, _aiVector.y, _aiVector.z);
		}

		static constexpr glm::vec2 toVec2XY(const aiVector3D& _aiVector)
		{
			return glm::vec2(_aiVector.x, _aiVector.y);
		}

		static constexpr Color toColor(const aiColor4D & _aiColor)
		{
			return Color(_aiColor.r, _aiColor.g, _aiColor.b, _aiColor.a);
		}

		bool createMeshInfo(const std::string _path, const aiMesh& _sceneMesh, MeshData::MeshInfo& _mesh)
		{
			_mesh.name = _sceneMesh.mName.C_Str();

			switch (_sceneMesh.mPrimitiveTypes)
			{
			case aiPrimitiveType::aiPrimitiveType_POINT:	_mesh.type = PrimitiveType::Points;		break;
			case aiPrimitiveType::aiPrimitiveType_LINE:		_mesh.type = PrimitiveType::Lines;		break;
			case aiPrimitiveType::aiPrimitiveType_TRIANGLE:	_mesh.type = PrimitiveType::Triangles;	break;
			default:
				log::error("Error loading model: " + _path + ", unknown primitive type!");
				return false;
			}

			_mesh.materialIndex = _sceneMesh.mMaterialIndex;

			using namespace VertexAttribute;
			VertexAttributeFlagsType attributes = Position;
			se_assert(_sceneMesh.HasPositions());
			se_assert(_sceneMesh.HasFaces());
			if (_sceneMesh.HasNormals())
				enableBit(attributes, Normal);
			if (_sceneMesh.HasTangentsAndBitangents())
				enableBit(attributes, Tangent | Bitangent);
			if (_sceneMesh.HasVertexColors(0))
				enableBit(attributes, Color0);
			if (_sceneMesh.HasVertexColors(1))
				enableBit(attributes, Color1);
			if (_sceneMesh.HasVertexColors(2))
				enableBit(attributes, Color2);
			if (_sceneMesh.HasVertexColors(3))
				enableBit(attributes, Color3);
			if (_sceneMesh.HasTextureCoords(0))
				enableBit(attributes, TexCoord0);
			if (_sceneMesh.HasTextureCoords(1))
				enableBit(attributes, TexCoord1);
			if (_sceneMesh.HasTextureCoords(2))
				enableBit(attributes, TexCoord2);
			se_assert_m(_sceneMesh.GetNumUVChannels() <= 3, "Max 3 UV channels are currently supported!");
			if (_sceneMesh.HasBones())
				enableBit(attributes, Weight | Indices);

			se_assert(_sceneMesh.mNumBones <= maxBones);
			_mesh.bones.resize(_sceneMesh.mNumBones);
			for (size_t boneIndex = 0; boneIndex < _sceneMesh.mNumBones; boneIndex++)
			{
				// NOTE: aiProcess_PopulateArmatureData
				//se_assert(_sceneMesh.mBones[boneIndex]->mName == _sceneMesh.mBones[boneIndex]->mNode->mName);
				_mesh.bones[boneIndex].boneNodeName = _sceneMesh.mBones[boneIndex]->mName.C_Str();
				//_mesh.bones[boneIndex].armatureNodeName = _sceneMesh.mBones[boneIndex]->mArmature->mName.C_Str();
				_mesh.bones[boneIndex].offsetMatrix = make_mat4(_sceneMesh.mBones[boneIndex]->mOffsetMatrix);
			}

			_mesh.vertexBuffer = std::make_shared<VertexBuffer>(attributes);
			_mesh.vertexBuffer->resize(_sceneMesh.mNumVertices);
			se_assert(_mesh.vertexBuffer->size() <= maxVertices);
			_mesh.indexBuffer = std::make_shared<IndexBuffer>();
			_mesh.indexBuffer->resize((size_t)_sceneMesh.mNumFaces * 3);
			se_assert(_mesh.indexBuffer->size() <= maxIndices);

			for (size_t vertexIndex = 0; vertexIndex < _sceneMesh.mNumVertices; vertexIndex++)
			{
				_mesh.vertexBuffer->get<Position>(vertexIndex) = toVec3(_sceneMesh.mVertices[vertexIndex]);
				if (checkBit(attributes, Normal))
					_mesh.vertexBuffer->get<Normal>(vertexIndex) = toVec3(_sceneMesh.mNormals[vertexIndex]);
				if (checkBit(attributes, Tangent))
					_mesh.vertexBuffer->get<Tangent>(vertexIndex) = toVec3(_sceneMesh.mTangents[vertexIndex]);
				if (checkBit(attributes, Bitangent))
					_mesh.vertexBuffer->get<Bitangent>(vertexIndex) = toVec3(_sceneMesh.mBitangents[vertexIndex]);
				if (checkBit(attributes, Color0))
					_mesh.vertexBuffer->get<Color0>(vertexIndex) = toColor(_sceneMesh.mColors[0][vertexIndex]);
				if (checkBit(attributes, Color1))
					_mesh.vertexBuffer->get<Color1>(vertexIndex) = toColor(_sceneMesh.mColors[1][vertexIndex]);
				if (checkBit(attributes, Color2))
					_mesh.vertexBuffer->get<Color2>(vertexIndex) = toColor(_sceneMesh.mColors[2][vertexIndex]);
				if (checkBit(attributes, Color3))
					_mesh.vertexBuffer->get<Color3>(vertexIndex) = toColor(_sceneMesh.mColors[3][vertexIndex]);
				if (checkBit(attributes, TexCoord0))
					_mesh.vertexBuffer->get<TexCoord0>(vertexIndex) = toVec2XY(_sceneMesh.mTextureCoords[0][vertexIndex]);
				if (checkBit(attributes, TexCoord1))
					_mesh.vertexBuffer->get<TexCoord1>(vertexIndex) = toVec2XY(_sceneMesh.mTextureCoords[1][vertexIndex]);
				if (checkBit(attributes, TexCoord2))
					_mesh.vertexBuffer->get<TexCoord2>(vertexIndex) = toVec2XY(_sceneMesh.mTextureCoords[2][vertexIndex]);
				if (checkBit(attributes, Weight))
				{
					se_assert(checkBit(attributes, Indices));
					int weightIndex = 0;
					for (size_t boneIndex = 0; boneIndex < _sceneMesh.mNumBones; boneIndex++)
					{
						for (size_t boneWeightIndex = 0; boneWeightIndex < _sceneMesh.mBones[boneIndex]->mNumWeights; boneWeightIndex++)
						{
							if (_sceneMesh.mBones[boneIndex]->mWeights[boneWeightIndex].mVertexId == vertexIndex)
							{
								_mesh.vertexBuffer->get<Indices>(vertexIndex)[weightIndex] = static_cast<int16_t>(boneIndex);
								_mesh.vertexBuffer->get<Weight>(vertexIndex)[weightIndex] = _sceneMesh.mBones[boneIndex]->mWeights[boneWeightIndex].mWeight;
								weightIndex++;
								break;
							}
						}

						se_assert(weightIndex <= maxBoneWeights);
						if (weightIndex >= maxBoneWeights)
							break;
					}

					for (; weightIndex < maxBoneWeights; weightIndex++)
					{
						_mesh.vertexBuffer->get<Indices>(vertexIndex)[weightIndex] = 0;
						_mesh.vertexBuffer->get<Weight>(vertexIndex)[weightIndex] = 0.0f;
					}
				}
			}
			size_t index = 0;
			for (size_t faceIndex = 0; faceIndex < _sceneMesh.mNumFaces; faceIndex++)
			{
				const aiFace& face = _sceneMesh.mFaces[faceIndex];
				for (unsigned int indexIndex = 0; indexIndex < face.mNumIndices; indexIndex++)
				{
					const unsigned int faceindex = face.mIndices[indexIndex];
					se_assert(faceindex < std::numeric_limits<IndexType>::max());
					_mesh.indexBuffer->operator[](index++) = static_cast<IndexType>(faceindex);
				}
			}

			return true;
		}

		std::unique_ptr<Animation> createAnimation(const aiAnimation& _aiAnimation)
		{
			std::unique_ptr<Animation> animation = std::make_unique<Animation>();
			animation->name = _aiAnimation.mName.C_Str();
			animation->numFrames = static_cast<int>(_aiAnimation.mDuration);
			animation->framesPerSeconds = _aiAnimation.mTicksPerSecond != 0.0 ? static_cast<float>(_aiAnimation.mTicksPerSecond) : 30.0f;

			for (size_t channelIndex = 0; channelIndex < _aiAnimation.mNumChannels; channelIndex++)
			{
				se_assert(_aiAnimation.mChannels[channelIndex]);
				const aiNodeAnim& aiNodeAnim = *_aiAnimation.mChannels[channelIndex];
				AnimationNode& animNode = animation->channels[aiNodeAnim.mNodeName.C_Str()];

				animNode.positionKeys.resize(aiNodeAnim.mNumPositionKeys);
				for (size_t posKey = 0; posKey < aiNodeAnim.mNumPositionKeys; posKey++)
				{
					animNode.positionKeys[posKey].first = static_cast<float>(aiNodeAnim.mPositionKeys[posKey].mTime);
					animNode.positionKeys[posKey].second = toVec3(aiNodeAnim.mPositionKeys[posKey].mValue);
				}

				animNode.rotationKeys.resize(aiNodeAnim.mNumRotationKeys);
				for (size_t rotKey = 0; rotKey < aiNodeAnim.mNumRotationKeys; rotKey++)
				{
					animNode.rotationKeys[rotKey].first = static_cast<float>(aiNodeAnim.mRotationKeys[rotKey].mTime);
					animNode.rotationKeys[rotKey].second = toQuat(aiNodeAnim.mRotationKeys[rotKey].mValue);
				}

				animNode.scalingKeys.resize(aiNodeAnim.mNumScalingKeys);
				for (size_t scaleKey = 0; scaleKey < aiNodeAnim.mNumScalingKeys; scaleKey++)
				{
					animNode.scalingKeys[scaleKey].first = static_cast<float>(aiNodeAnim.mScalingKeys[scaleKey].mTime);
					animNode.scalingKeys[scaleKey].second = toVec3(aiNodeAnim.mScalingKeys[scaleKey].mValue);
				}
			}

			return animation;
		}

		std::shared_ptr<ResourceData> ModelData::createResource(const std::string _path)
		{
			Assimp::Importer importer;
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, maxVertices);
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, maxIndices / 3);
			importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, maxBoneWeights);
			importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES, maxBones);
			constexpr unsigned sceneReadFlags =
				aiProcess_CalcTangentSpace |
				aiProcess_JoinIdenticalVertices |
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_SplitLargeMeshes |
				aiProcess_LimitBoneWeights |
				aiProcess_ValidateDataStructure |
				// aiProcess_FixInfacingNormals |
				// aiProcess_PopulateArmatureData |
				aiProcess_SortByPType |
				aiProcess_FindDegenerates |
				aiProcess_FindInvalidData |
				aiProcess_GenUVCoords |
				// aiProcess_OptimizeMeshes |
				// aiProcess_OptimizeGraph |
				aiProcess_FlipUVs |
				aiProcess_FlipWindingOrder |		// No idea why this is needed, SpehsEngine should be CCW by default
				aiProcess_SplitByBoneCount |
				aiProcess_GlobalScale;
			const aiScene* scene = importer.ReadFile(_path, sceneReadFlags);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				log::error("Error loading model: " + _path + " | Assimp error: " + importer.GetErrorString());
				return nullptr;
			}

			if (scene->mNumTextures > 0)
				log::warning(_path + ": Embedded textures not supported!");
			if (scene->mNumLights > 0)
				log::warning(_path + ": Scene lights not supported!");
			if (scene->mNumCameras > 0)
				log::warning(_path + ": Scene cameras not supported!");

			if (false && scene->mMetaData) // Print metadata
			{
				createResourcePrintMetadata(_path, *scene->mMetaData);
			}

			std::shared_ptr<MeshData> resource = std::make_shared<MeshData>();

			resource->meshes.resize(scene->mNumMeshes);
			for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
			{
				const aiMesh& sceneMesh = *scene->mMeshes[meshIndex];
				MeshData::MeshInfo& mesh = resource->meshes[meshIndex];
				if (!createMeshInfo(_path, sceneMesh, mesh))
				{
					return nullptr;
				}
			}

			if (scene->mNumAnimations > 0)
			{
				resource->animations = std::make_shared<std::vector<std::unique_ptr<Animation>>>();
				resource->animations->resize(scene->mNumAnimations);
				for (size_t animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
				{
					const aiAnimation& sceneAnim = *scene->mAnimations[animationIndex];
					resource->animations->at(animationIndex) = createAnimation(sceneAnim);
				}
			}

			resource->globalInverseMatrix = glm::inverse(make_mat4(scene->mRootNode->mTransformation));

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
