#pragma once

namespace se::gfx::impl
{
	class LightBatch;
	class LightInternal;
}

namespace se::gfx
{
	class Primitive;

	class Light
	{
	public:

											Light() = default;
		virtual 							~Light();

											Light(const Light& _other) = delete;
		Light&								operator=(const Light& _other) = delete;

											Light(Light&& _other) = delete;
		Light&								operator=(Light&& _other) = delete;

		void								setColor(const Color& _color);
		void								setIntensity(const float _intensity);
		void								setPosition(const glm::vec3& _position);
		void								setGlobal(const bool _global);
		void								setDirection(const glm::vec3& _direction);
		void								setRadius(const float _inner, const float _outer);
		void								setCone(const float _inner, const float _outer);

		const Color&						getColor() const;
		const float&						getIntensity() const;
		const glm::vec3&					getPosition() const;
		const bool							getGlobal() const;
		const glm::vec3&					getDirection() const;
		const float							getInnerRadius() const;
		const float							getOuterRadius() const;
		const float							getInnerCone() const;
		const float							getOuterCone() const;

		virtual std::shared_ptr<Primitive>	getDebugPrimitive();

	protected:

		friend class impl::LightBatch;
		friend class impl::LightInternal;

		// mat4[0]
		Color color;				// Color, alpha is intensity

		// mat4[1]
		glm::vec3 position;
		float global = 0.0f;		// 0.0f = light position applies, 1.0f = light applies globally

		// mat4[2]
		glm::vec3 direction;
		float __unused;

		// mat4[3]
		float innerRadius;			// Attenuation Radius
		float outerRadius;
		float innerCone;			// Spotlight cone angle (radians)
		float outerCone;

		// extra
		bool dirty = true;
		std::shared_ptr<Primitive> debugPrimitive;
		virtual void generateDebugPrimitive() {}

		boost::signals2::signal<void(void)> destroyedSignal;
	};


	class AmbientLight final : public Light
	{
	public:

		AmbientLight();
		AmbientLight(const Color& _color, const float _intensity);
	};


	class DirectionalLight final : public Light
	{
	public:

		DirectionalLight();
		DirectionalLight(const Color& _color, const float _intensity, const glm::vec3& _direction);
	};


	class PointLight final : public Light
	{
	public:

		PointLight();
		PointLight(const Color& _color, const float _intensity, const glm::vec3& _position,
				   const float _innerRadius, const float _outerRadius);

	private:

		void generateDebugPrimitive() override;
	};


	class SpotLight final : public Light
	{
	public:

		SpotLight();
		SpotLight(const Color& _color, const float _intensity,
				  const glm::vec3& _position, const glm::vec3& _direction,
				  const float _innerRadius, const float _outerRadius,
				  const float _innerCone, const float _outerCone);

	private:

		void generateDebugPrimitive() override;
	};
}
