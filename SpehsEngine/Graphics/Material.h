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
			virtual std::shared_ptr<Texture> getTexture(const uint8_t _slot) const final;
			virtual std::shared_ptr<Font> getFont(const uint8_t _slot) const final;

			void connectToFontChangedSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(void)>& callback);

			virtual void setShader(std::shared_ptr<Shader> _shader) final;
			virtual void setTexture(std::shared_ptr<Texture> _texture, const std::string_view _uniformName, const uint8_t _slot) final;
			virtual void setFont(std::shared_ptr<Font> _font, const std::string_view _uniformName, const uint8_t _slot) final;

			void setLit(const bool _value);
			const bool getLit() const;

		protected:

			virtual void internalBind() = 0;

		private:

			struct MaterialTexture
			{
				std::unique_ptr<Uniform> uniform;
				std::shared_ptr<Texture> texture;
			};
			struct MaterialFont
			{
				std::unique_ptr<Uniform> uniform;
				std::shared_ptr<Font> font;
			};

			std::shared_ptr<Shader> shader;
			std::unordered_map<uint8_t, std::unique_ptr<MaterialTexture>> textures;
			std::unordered_map<uint8_t, std::unique_ptr<MaterialFont>> fonts;

			bool lit = false;

			boost::signals2::signal<void(void)> fontChangedSignal;
		};
	}
}
