
#pragma once

#include "SpehsEngine/Core/Color.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

#include <vector>


namespace se
{
	namespace rendering
	{
		class Mesh;
		class BatchManager3D;
		class Line3D
		{
			friend class BatchManager3D;

		public:
			Line3D();
			~Line3D();

			void addPoint(const glm::vec3& _point);
			void addPoints(const std::vector<glm::vec3>& _points);
			void updatePoint(const size_t _index, const glm::vec3& _point);
			void updatePoints(const std::vector<glm::vec3>& _points);
			void setPointColor(const size_t _index, const Color _color);
			se::Color getPointColor(const size_t _index);
			void clearPoints();

			glm::vec3 getPoint(const size_t _index);

			void setPosition(const glm::vec3& _newPosition);
			void setRotation(const glm::quat& _newRotation);
			void setScale(const glm::vec3& _newScale);
			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void translate(const glm::vec3& _translation);

			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setShaderIndex(const unsigned int _newShaderIndex);
			void setLineWidth(const float _value);
			void setLineSmoothing(const bool _value);

			glm::vec3 getPosition() const;
			glm::quat getRotation() const;
			glm::vec3 getScale() const;
			se::Color getColor() const;
			float getAlpha() const;

			bool getRenderState() const;
			bool getBlending() const;
			bool getDepthTest() const;
			unsigned int getShaderIndex() const;
			float getLineWidth() const;
			bool getLineSmoothing() const;

		protected:
			BatchManager3D* batchManager = nullptr;
			Mesh* mesh;

		private:
			Line3D(const Line3D& _other) = delete;
			Line3D(const Line3D&& _other) = delete;
			Line3D& operator=(const Line3D& _other) = delete;
			Line3D& operator=(const Line3D&& _other) = delete;
		};
	}
}