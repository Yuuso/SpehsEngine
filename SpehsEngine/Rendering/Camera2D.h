#pragma once

#include <glm/mat4x4.hpp>
#include <boost/signal.hpp>
#include "SpehsEngine/Core/Vector.h"


namespace spehs
{
	class Window;
	class Camera2D
	{
	public:
		Camera2D(const Window& window);
		~Camera2D();

		void update();

		void translate(const spehs::vec2& _vec);
		void setRotation(const float &_rotation);
		
		//Public Variables
		const Window& window;
		spehs::vec2 deltaMovement;
		glm::mat4* projectionMatrix;
		glm::mat4 staticMatrix;

		float scale;
		spehs::vec2 position;
		spehs::vec2 previousPosition;

		void windowSizeChangedCallback(const int width, const int height);
	private:

		float rotation;
		float zoomSpeed;

		spehs::vec4 up; //compensate for rotation

		glm::mat4 orthoMatrix;
		glm::mat4 defaultMatrix;
		glm::mat4 cameraMatrix;

		boost::signals::scoped_connection windowSizeChangedConnection;
	};
}