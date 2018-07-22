
#pragma once


namespace se
{
	namespace rendering
	{
		class Mesh;
		class BatchManager3D;
		class Plane3D
		{
			friend class BatchManager3D;

		public:
			Plane3D();
			~Plane3D();

			void setPosition(const glm::vec3& _newPosition);
			void setRotation(const glm::vec3& _newRotation);
			void setScale(const glm::vec3& _newScale);
			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void translate(const glm::vec3& _translation);

			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setShaderIndex(const unsigned int _newShaderIndex);

			void setTexture(const std::string& _texturePath);
			void setTexture(const size_t _textureID);
			void setTexture(TextureData* _textureDataPtr);

			glm::vec3 getPosition() const;
			glm::vec3 getRotation() const;
			glm::vec3 getScale() const;
			se::Color getColor() const;
			float getAlpha() const;

			bool getRenderState() const;
			bool getBlending() const;
			bool getDepthTest() const;
			unsigned int getShaderIndex() const;

		protected:
			BatchManager3D* batchManager = nullptr;
			Mesh* mesh;

		private:
			Plane3D(const Plane3D& _other) = delete;
			Plane3D(const Plane3D&& _other) = delete;
			Plane3D& operator=(const Plane3D& _other) = delete;
			Plane3D& operator=(const Plane3D&& _other) = delete;
		};
	}
}
