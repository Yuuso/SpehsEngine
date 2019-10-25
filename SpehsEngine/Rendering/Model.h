
#pragma once

#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>
#include <memory>


namespace se
{
	namespace rendering
	{
		class Mesh;
		class TextureManager;
		struct TextureData;

		class Model
		{
			friend class BatchManager3D;
			friend class ModelManager;
			friend class Skybox;

		public:
			Model();
			~Model();

			void loadModel(ModelManager& _modelManager, const std::string& _filepath);
			void loadModel(ModelManager& _modelManager, const size_t _hash);


			// --------------------
			// Mesh Functions
			// --------------------

			void addMesh(const Mesh& _mesh);
			void insertMesh(const size_t _position, const Mesh& _mesh);
			void replaceMesh(const size_t _position, const Mesh& _mesh);
			std::vector<Mesh*> findMeshes(const std::string& _name) const;
			std::vector<Mesh*> findMeshesContains(const std::string& _partialName) const;
			std::vector<std::unique_ptr<Mesh>>& getMeshes();
			const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;
			Mesh* getMesh(size_t _index) const;
			void removeMeshes(const std::string& _name);
			void removeMesh(const size_t _index);

			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void setBackFaceCulling(const bool _value);
			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setShaderIndex(const unsigned int _newShaderIndex);
			void setStaticDraw(const bool _value);
			void setTexture(TextureManager& _textureManager, const std::string& _texturePath, const size_t _index = 0);
			void setTexture(TextureManager& _textureManager, const size_t _textureID, const size_t _index = 0);
			void setTexture(TextureData& _textureDataPtr, const size_t _index = 0);


			// --------------------
			// Model Functions
			// --------------------

			void setPosition(const glm::vec3& _newPosition);
			void setRotation(const glm::quat& _newRotation);
			void setScale(const glm::vec3& _newScale);
			void translate(const glm::vec3& _translation);

			const glm::vec3& getPosition() const { return position; };
			const glm::quat& getRotation() const { return rotation; };
			const glm::vec3& getScale() const { return scale; };
			const se::Color& getColor() const { return modelColor; }


		protected:
			se::Color modelColor;

			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			std::vector<BatchManager3D*> batchManagers;
			std::vector<std::unique_ptr<Mesh>> meshes;

		private:
			Model(const Model& _other) = delete;
			Model(const Model&& _other) = delete;
			Model& operator=(const Model& _other) = delete;
			Model& operator=(const Model&& _other) = delete;

			void updateMeshTransform(Mesh& _mesh);
		};
	}
}