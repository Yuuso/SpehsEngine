
#pragma once

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

			// Setters
			void setPosition(const float _x, const float _y, const float _z);
			void setPosition(const glm::vec3& _newPosition);
			void setPosition(const Mesh& _newPosition);
			void setRotation(const float _yaw, const float _pitch, const float _roll);
			void setRotation(const glm::vec3& _newRotation);
			void setScale(const float _newScale);
			void setScale(const float _newScaleX, const float _newScaleY, const float _newScaleZ);
			void setScale(const glm::vec3& _newScale);
			void setColor(const Mesh& _other);
			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void translate(const float _x, const float _y, const float _z);
			void translate(const glm::vec3& _translation);
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setRenderState(const Mesh& _other);
			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setShaderIndex(const unsigned int _newShaderIndex);
			void setBackFaceCulling(const bool _value);
			void setTexture(const std::string& _texturePath);
			void setTexture(const size_t _textureID);
			void setTexture(TextureData* _textureDataPtr);

			// Getters
			glm::vec3 getPosition() const;
			glm::vec3 getRotation() const;
			glm::vec3 getScale() const;
			bool getRenderState() const;
			unsigned int getShaderIndex() const;
			se::Color getColor() const;
			float getAlpha() const;

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