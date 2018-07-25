
#pragma once

#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>


namespace se
{
	namespace rendering
	{
		class Mesh;
		class Model
		{
			friend class BatchManager3D;
			friend class ModelManager;
			friend class Skybox;

		public:
			Model();
			Model(BatchManager3D& _batchManager);
			~Model();

			void loadModel(const std::string& _filepath);
			void loadModel(const size_t _hash);

			void setPosition(const glm::vec3& _newPosition);
			void setRotation(const glm::quat& _newRotation);
			void setScale(const glm::vec3& _newScale);
			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void translate(const glm::vec3& _translation);

			void setBackFaceCulling(const bool _value);
			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setShaderIndex(const unsigned int _newShaderIndex);

			void setTexture(const std::string& _texturePath, const size_t _index = 0);
			void setTexture(const size_t _textureID, const size_t _index = 0);
			void setTexture(TextureData* _textureDataPtr, const size_t _index = 0);

			glm::vec3 getPosition() const;
			glm::quat getRotation() const;
			glm::vec3 getScale() const;
			se::Color getColor() const;
			float getAlpha() const;

			bool getBackFaceCulling() const;
			bool getRenderState() const;
			bool getBlending() const;
			bool getDepthTest() const;
			unsigned int getShaderIndex() const;

		protected:
			BatchManager3D* batchManager = nullptr;
			std::vector<Mesh*> meshes;

		private:
			Model(const Model& _other) = delete;
			Model(const Model&& _other) = delete;
			Model& operator=(const Model& _other) = delete;
			Model& operator=(const Model&& _other) = delete;

			void clearMeshes();
			void addMeshes();
		};
	}
}