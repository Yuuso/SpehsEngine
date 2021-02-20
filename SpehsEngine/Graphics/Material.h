#pragma once

#include <memory>
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Font.h"


namespace se
{
	namespace graphics
	{
		class Material
		{
		public:

			virtual ~Material() = default;

			// All shader uniform values should be bound here
			virtual void bind() = 0;

			// All materials contain a shader
			virtual std::shared_ptr<Shader> getShader() = 0;

			virtual std::shared_ptr<Texture> getTexture([[maybe_unused]] const size_t _index = 0) { return nullptr; }
			virtual std::shared_ptr<Font> getFont([[maybe_unused]] const size_t _index = 0) { return nullptr; }
		};
	}
}
