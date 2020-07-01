#pragma once

#include <memory>
#include "SpehsEngine/Graphics/Shader.h"


namespace se
{
	namespace graphics
	{
		class Material
		{
		public:

			virtual ~Material() = default;

			// All shader uniform values should be bound here
			virtual void update() = 0;

			// All materials contain a shader
			virtual std::shared_ptr<Shader> getShader() = 0;
		};
	}
}
