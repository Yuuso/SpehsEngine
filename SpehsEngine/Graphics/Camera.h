#pragma once


namespace se::gfx
{
	class Camera
	{
	public:

		Projection			getProjection() const;
		const glm::vec3&	getPosition() const;
		const glm::vec3		getDirection() const;
		const glm::vec3&	getTarget() const;
		const glm::vec3&	getUp() const;
		const glm::vec3		getLeft() const;
		float				getZoom() const;
		float				getFov() const;
		float				getNear() const;
		float				getFar() const;

		void				setProjection(const Projection _projection);
		void				setPosition(const glm::vec3& _position);
		void				setDirection(const glm::vec3& _direction);
		void				setTarget(const glm::vec3& _target);
		void				setUp(const glm::vec3& _up);
		void				setZoom(const float _zoom);
		void				setFov(const float _fov);
		void				setNear(const float _near);
		void				setFar(const float _far);

		glm::mat4			getViewMatrix();
		glm::mat4			getProjectionMatrix(const float _viewWidth, const float _viewHeight);

	private:

		Projection			projection		= Projection::Perspective;
		bool				followTarget	= false;
		glm::vec3			up				= glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3			position		= glm::vec3(0.0f, 10.0f, -15.0f);
		glm::vec3			target			= glm::vec3(0.0f, 0.0f, 0.0f);
		float				zoom			= 1.0f;
		float				fov				= 60.0f;
		float				nearPlane		= 0.05f;
		float				farPlane		= 1000.0f;
	};
}
