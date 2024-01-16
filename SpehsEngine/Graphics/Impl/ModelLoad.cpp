#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/ModelLoad.h"

#include "SpehsEngine/Graphics/Impl/AssetData.h"
#include "SpehsEngine/Graphics/Impl/Animation.h"
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


namespace se::gfx::impl
{
	inline static glm::mat4 make_mat4(const aiMatrix4x4& _matrix)
	{
		return glm::transpose(glm::make_mat4(&_matrix.a1));
	}
	inline static glm::vec2 to_vec2(const aiVector3D& _vec)
	{
		return glm::vec2{ _vec.x, _vec.y };
	}
	inline static glm::vec3 to_vec3(const aiVector3D& _vec)
	{
		return glm::vec3{ _vec.x, _vec.y, _vec.z };
	}
	inline static Color to_color(const aiColor4D& _color)
	{
		return Color{ _color.r, _color.g, _color.b, _color.a };
	}
	static void processNode(MeshData::MeshDataNode& _modelNode, const aiNode& _node)
	{
		_modelNode.name = _node.mName.C_Str();
		_modelNode.transform = make_mat4(_node.mTransformation);

		_modelNode.meshIndices.resize(_node.mNumMeshes);
		for (size_t i = 0; i < _node.mNumMeshes; i++)
		{
			_modelNode.meshIndices[i] = _node.mMeshes[i];
		}

		_modelNode.children.resize(_node.mNumChildren);
		for (unsigned int i = 0; i < _node.mNumChildren; i++)
		{
			_modelNode.children[i] = std::make_unique<MeshData::MeshDataNode>();
			processNode(*_modelNode.children[i].get(), *_node.mChildren[i]);
		}
	}
	std::shared_ptr<AssetData> loadModel(std::string _path)
	{
		Assimp::Importer importer;
		constexpr int maxVertices = UINT16_MAX;
		constexpr int maxIndices = UINT16_MAX;
		constexpr int maxBoneWeights = 4;
		constexpr int maxBones = (int)SE_MAX_BONES; // Max bones per mesh, also defined by the shaders, note: aiProcess_SplitByBoneCount
		importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, maxVertices);
		importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, maxIndices / 3);
		importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, maxBoneWeights);
		importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES, maxBones);
		const aiScene* scene = importer.ReadFile(
			  _path,
			  aiProcess_CalcTangentSpace
			| aiProcess_JoinIdenticalVertices
			| aiProcess_Triangulate
			| aiProcess_GenSmoothNormals
			| aiProcess_SplitLargeMeshes
			| aiProcess_LimitBoneWeights
			| aiProcess_ValidateDataStructure
		  //| aiProcess_FixInfacingNormals
		  //| aiProcess_PopulateArmatureData
			| aiProcess_SortByPType
			| aiProcess_FindDegenerates
			| aiProcess_FindInvalidData
			| aiProcess_GenUVCoords
		  //| aiProcess_OptimizeMeshes
		  //| aiProcess_OptimizeGraph
			| aiProcess_FlipUVs
			| aiProcess_FlipWindingOrder // No idea why this is needed, SpehsEngine should be CCW by default
			| aiProcess_SplitByBoneCount
			| aiProcess_GlobalScale
		);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			log::error("Error loading model: " + _path + ", Assimp error: " + importer.GetErrorString());
			return nullptr;
		}

		if (scene->mNumTextures > 0)
		{
			log::warning(_path + ": Embedded textures not supported!");
		}
		if (scene->mNumLights > 0)
		{
			log::warning(_path + ": Scene lights not supported!");
		}
		if (scene->mNumCameras > 0)
		{
			log::warning(_path + ": Scene cameras not supported!");
		}

		// Print metadata
	#if false
		if (scene->mMetaData)
		{
			#define PRINT_METADATA(TYPE, TOSTRING) { \
				const TYPE metaDataValue = *reinterpret_cast<TYPE*>(scene->mMetaData->mValues[metaDataIndex].mData); \
				log::info(_path + ", metadata: " + scene->mMetaData->mKeys[metaDataIndex].C_Str() + " = " + TOSTRING(metaDataValue)); \
			}
			auto aiStringToString = [](const aiString& s){ return s.C_Str(); };
			auto aiVectorToString = [](const aiVector3D& v){ return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z); };

			for (size_t metaDataIndex = 0; metaDataIndex < scene->mMetaData->mNumProperties; metaDataIndex++)
			{
				switch (scene->mMetaData->mValues[metaDataIndex].mType)
				{
					default: break;
					case AI_BOOL:		PRINT_METADATA(bool,		se::toString);		break;
					case AI_INT32:		PRINT_METADATA(int32_t,		std::to_string);	break;
					case AI_UINT64:		PRINT_METADATA(uint64_t,	std::to_string);	break;
					case AI_FLOAT:		PRINT_METADATA(float,		std::to_string);	break;
					case AI_DOUBLE:		PRINT_METADATA(double,		std::to_string);	break;
					case AI_AISTRING:	PRINT_METADATA(aiString,	aiStringToString);	break;
					case AI_AIVECTOR3D: PRINT_METADATA(aiVector3D,	aiVectorToString);	break;
				}
			}
		}
	#endif

		std::shared_ptr<MeshData> asset = std::make_shared<MeshData>();

		// Mesh data
		asset->meshes.resize(scene->mNumMeshes);
		for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
		{
			const aiMesh& sceneMesh = *scene->mMeshes[meshIndex];
			MeshData::MeshInfo& mesh = asset->meshes[meshIndex];

			mesh.name = sceneMesh.mName.C_Str();

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
			se_assert_m(sceneMesh.GetNumUVChannels() <= 3, "Max 3 UV channels are currently supported!");
			if (sceneMesh.HasNormals())					enableBit(attributes, Normal);
			if (sceneMesh.HasTangentsAndBitangents())	enableBit(attributes, Tangent | Bitangent);
			if (sceneMesh.HasVertexColors(0))			enableBit(attributes, Color0);
			if (sceneMesh.HasVertexColors(1))			enableBit(attributes, Color1);
			if (sceneMesh.HasVertexColors(2))			enableBit(attributes, Color2);
			if (sceneMesh.HasVertexColors(3))			enableBit(attributes, Color3);
			if (sceneMesh.HasTextureCoords(0))			enableBit(attributes, TexCoord0);
			if (sceneMesh.HasTextureCoords(1))			enableBit(attributes, TexCoord1);
			if (sceneMesh.HasTextureCoords(2))			enableBit(attributes, TexCoord2);
			if (sceneMesh.HasBones())					enableBit(attributes, Weight | Indices);

			se_assert(sceneMesh.mNumBones <= maxBones);
			mesh.bones.resize(sceneMesh.mNumBones);
			for (size_t boneIndex = 0; boneIndex < sceneMesh.mNumBones; boneIndex++)
			{
				// NOTE: aiProcess_PopulateArmatureData
				//se_assert(sceneMesh.mBones[boneIndex]->mName == sceneMesh.mBones[boneIndex]->mNode->mName);
				mesh.bones[boneIndex].boneNodeName = sceneMesh.mBones[boneIndex]->mName.C_Str();
				//mesh.bones[boneIndex].armatureNodeName = sceneMesh.mBones[boneIndex]->mArmature->mName.C_Str();
				mesh.bones[boneIndex].offsetMatrix = make_mat4(sceneMesh.mBones[boneIndex]->mOffsetMatrix);
			}

			mesh.vertexBuffer = std::make_shared<VertexBuffer>(attributes);
			mesh.vertexBuffer->resize(sceneMesh.mNumVertices);
			se_assert(mesh.vertexBuffer->size() <= maxVertices);
			mesh.indexBuffer = std::make_shared<IndexBuffer>();
			mesh.indexBuffer->resize((size_t)sceneMesh.mNumFaces * 3);
			se_assert(mesh.indexBuffer->size() <= maxIndices);

			// Vertex data
			for (size_t vertexIndex = 0; vertexIndex < sceneMesh.mNumVertices; vertexIndex++)
			{
			#define SET_ATTR_IF(ATTR, VALUE) \
				if (checkBit(attributes, ATTR))	mesh.vertexBuffer->set<ATTR>(vertexIndex, VALUE)

				SET_ATTR_IF(Position,		to_vec3(sceneMesh.mVertices[vertexIndex])			);
				SET_ATTR_IF(Normal,			to_vec3(sceneMesh.mNormals[vertexIndex])			);
				SET_ATTR_IF(Tangent,		to_vec3(sceneMesh.mTangents[vertexIndex])			);
				SET_ATTR_IF(Bitangent,		to_vec3(sceneMesh.mBitangents[vertexIndex])			);
				SET_ATTR_IF(Color0,			to_color(sceneMesh.mColors[0][vertexIndex])			);
				SET_ATTR_IF(Color1,			to_color(sceneMesh.mColors[1][vertexIndex])			);
				SET_ATTR_IF(Color2,			to_color(sceneMesh.mColors[2][vertexIndex])			);
				SET_ATTR_IF(Color3,			to_color(sceneMesh.mColors[3][vertexIndex])			);
				SET_ATTR_IF(TexCoord0,		to_vec2(sceneMesh.mTextureCoords[0][vertexIndex])	);
				SET_ATTR_IF(TexCoord1,		to_vec2(sceneMesh.mTextureCoords[1][vertexIndex])	);
				SET_ATTR_IF(TexCoord2,		to_vec2(sceneMesh.mTextureCoords[2][vertexIndex])	);

				if (checkBit(attributes, Weight))
				{
					se_assert(checkBit(attributes, Indices));
					int weightIndex = 0;
					for (unsigned int boneIndex = 0; boneIndex < sceneMesh.mNumBones; boneIndex++)
					{
						for (unsigned int boneWeightIndex = 0; boneWeightIndex < sceneMesh.mBones[boneIndex]->mNumWeights; boneWeightIndex++)
						{
							if (sceneMesh.mBones[boneIndex]->mWeights[boneWeightIndex].mVertexId == vertexIndex)
							{
								mesh.vertexBuffer->get<Indices>(vertexIndex)[weightIndex] = static_cast<int16_t>(boneIndex);
								mesh.vertexBuffer->get<Weight>(vertexIndex)[weightIndex] = sceneMesh.mBones[boneIndex]->mWeights[boneWeightIndex].mWeight;
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
						mesh.vertexBuffer->get<Indices>(vertexIndex)[weightIndex] = 0;
						mesh.vertexBuffer->get<Weight>(vertexIndex)[weightIndex] = 0.0f;
					}
				}
			}

			size_t indexBufferIndex = 0;
			for (size_t faceIndex = 0; faceIndex < sceneMesh.mNumFaces; faceIndex++)
			{
				const aiFace& face = sceneMesh.mFaces[faceIndex];
				for (unsigned int indicesIndex = 0; indicesIndex < face.mNumIndices; indicesIndex++)
				{
					const unsigned int index = face.mIndices[indicesIndex];
					se_assert(index <= std::numeric_limits<IndexType>::max());
					mesh.indexBuffer->operator[](indexBufferIndex++) = static_cast<IndexType>(index);
				}
			}
		}

		// Animations
		if (scene->mNumAnimations > 0)
		{
			asset->animations = std::make_shared<std::vector<std::unique_ptr<Animation>>>();
			asset->animations->resize(scene->mNumAnimations);
		}
		for (size_t animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
		{
			const aiAnimation& sceneAnim = *scene->mAnimations[animationIndex];
			std::unique_ptr<Animation>& animation = asset->animations->at(animationIndex);
			animation = std::make_unique<Animation>();

			animation->name = sceneAnim.mName.C_Str();
			animation->numFrames = static_cast<int>(sceneAnim.mDuration);
			animation->framesPerSeconds = sceneAnim.mTicksPerSecond != 0.0 ? static_cast<float>(sceneAnim.mTicksPerSecond) : 30.0f;

			for (size_t channelIndex = 0; channelIndex < sceneAnim.mNumChannels; channelIndex++)
			{
				AnimationNode& animNode = animation->channels[sceneAnim.mChannels[channelIndex]->mNodeName.C_Str()];

				animNode.positionKeys.resize(sceneAnim.mChannels[channelIndex]->mNumPositionKeys);
				for (size_t posKey = 0; posKey < sceneAnim.mChannels[channelIndex]->mNumPositionKeys; posKey++)
				{
					animNode.positionKeys[posKey].first = static_cast<float>(sceneAnim.mChannels[channelIndex]->mPositionKeys[posKey].mTime);
					animNode.positionKeys[posKey].second = glm::make_vec3(&sceneAnim.mChannels[channelIndex]->mPositionKeys[posKey].mValue.x);
				}

				animNode.rotationKeys.resize(sceneAnim.mChannels[channelIndex]->mNumRotationKeys);
				for (size_t rotKey = 0; rotKey < sceneAnim.mChannels[channelIndex]->mNumRotationKeys; rotKey++)
				{
					animNode.rotationKeys[rotKey].first = static_cast<float>(sceneAnim.mChannels[channelIndex]->mRotationKeys[rotKey].mTime);
					animNode.rotationKeys[rotKey].second = glm::quat(
						sceneAnim.mChannels[channelIndex]->mRotationKeys[rotKey].mValue.w,
						sceneAnim.mChannels[channelIndex]->mRotationKeys[rotKey].mValue.x,
						sceneAnim.mChannels[channelIndex]->mRotationKeys[rotKey].mValue.y,
						sceneAnim.mChannels[channelIndex]->mRotationKeys[rotKey].mValue.z);
				}

				animNode.scalingKeys.resize(sceneAnim.mChannels[channelIndex]->mNumScalingKeys);
				for (size_t scaleKey = 0; scaleKey < sceneAnim.mChannels[channelIndex]->mNumScalingKeys; scaleKey++)
				{
					animNode.scalingKeys[scaleKey].first = static_cast<float>(sceneAnim.mChannels[channelIndex]->mScalingKeys[scaleKey].mTime);
					animNode.scalingKeys[scaleKey].second = glm::make_vec3(&sceneAnim.mChannels[channelIndex]->mScalingKeys[scaleKey].mValue.x);
				}
			}
		}

		asset->globalInverseMatrix = glm::inverse(make_mat4(scene->mRootNode->mTransformation));
		processNode(asset->rootNode, *scene->mRootNode);

		return asset;
	}
}
