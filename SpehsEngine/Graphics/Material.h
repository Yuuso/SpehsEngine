#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include <memory>
#include <vector>


namespace se
{
	namespace graphics
	{
		class Material
		{
		public:

			Material() = default;
			virtual ~Material() = default;

			Material(const Material& _other) = delete;
			Material& operator=(const Material& _other) = delete;

			Material(Material&& _other) = delete;
			Material& operator=(Material&& _other) = delete;


			virtual void bind() final;

			virtual std::shared_ptr<Shader> getShader() const final;
			virtual std::shared_ptr<Font> getFont() const final;
			virtual std::shared_ptr<Texture> getTexture(const std::string& _name) const final;

			void connectToFontChangedSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(void)>& callback);

			virtual void setShader(std::shared_ptr<Shader> _shader) final;
			virtual void setFont(std::shared_ptr<Font> _font) final;
			virtual void setTexture(std::shared_ptr<Texture> _texture, const std::string& _name) final;

		protected:

			virtual void internalBind(uint8_t stage) = 0;

		private:

			struct MaterialTexture
			{
				std::unique_ptr<Uniform> uniform;
				std::shared_ptr<Texture> texture;
			};

			std::shared_ptr<Shader> shader;
			std::unique_ptr<Uniform> fontUniform;
			std::shared_ptr<Font> font;
			std::unordered_map<std::string, std::unique_ptr<MaterialTexture>> textures;

			boost::signals2::signal<void(void)> fontChangedSignal;
		};
	}
}
