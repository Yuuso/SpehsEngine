
#include "SpehsEngine/Rendering/Camera2D.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Input/Input.h"

#include <boost/bind.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>



namespace se
{
	Camera2D::Camera2D(const Window& _window)
		: window(_window)
		, zoomSpeed(50.0f)
		, scale(1.0f)
		, orthoMatrix(1.0f)
		, cameraMatrix(1.0f)
		, position(0.0f, 0.0f)
		, previousPosition(0.0f, 0.0f)
		, rotation(0.0f)
		, up(0.0f, 1.0f, 0.0f, 1.0f)
	{
		//windowSizeChangedConnection = window.sizeChangedSignal.connect(boost::bind(&Camera2D::windowSizeChangedCallback, this));
		windowSizeChangedCallback(window.getWidth(), window.getHeight());
	}

	Camera2D::~Camera2D()
	{
	}

	void Camera2D::windowSizeChangedCallback(const int width, const int height)
	{
		orthoMatrix = glm::ortho(0.0f, (float)window.getWidth(), 0.0f, (float)window.getHeight());
		staticMatrix = orthoMatrix;

		//Camera translation
		glm::vec3 translation((float)window.getWidth() * 0.5f, (float)window.getHeight() * 0.5f, 0);
		cameraMatrix = glm::translate(orthoMatrix, translation);

		defaultMatrix = cameraMatrix;
		projectionMatrix = &cameraMatrix;
	}

	void Camera2D::translate(const se::vec2& _vec)
	{
		position += _vec;
		//?rotation
	}

	void Camera2D::setRotation(const float &_rotation)
	{
		rotation = _rotation;
		//up = glm::normalize(glm::rotate(glm::mat4(), rotation, se::vec3(0.0f, 0.0f, 1.0f)) * up);
	}

	void Camera2D::update()
	{
		//1. Rotation
		cameraMatrix = glm::rotate(defaultMatrix, -rotation, glm::vec3(0.0f, 0.0f, 1.0f));

		//2. Scaling
		cameraMatrix = glm::scale(cameraMatrix, glm::vec3(scale, scale, 0.0f));

		//3. Translation
		cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-position.x, -position.y, 0.0f));

		//Delta movement
		deltaMovement = position - previousPosition;
		previousPosition = position;
	}
}