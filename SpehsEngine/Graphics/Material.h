#pragma once

#include "boost/signals2.hpp"
#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/ShaderVariant.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include <memory>
#include <vector>


namespace se
{
	namespace graphics
	{
		class UniformContainer
		{
		public:
			virtual ~UniformContainer() = default;
			virtual void bind() = 0;
		};


		class Material final
		{
		public:

										Material() = default;
										~Material() = default;

										Material(const Material& _other) = delete;
			Material&					operator=(const Material& _other) = delete;

										Material(Material&& _other) = delete;
			Material&					operator=(Material&& _other) = delete;


			void						bind();
			void						addUniformContainer(std::shared_ptr<UniformContainer> _uniformContainer);
			void						connectToFontChangedSignal(boost::signals2::scoped_connection& scopedConnection, const boost::function<void(void)>& callback);

			std::shared_ptr<Shader>		getShader(ShaderVariant variant = ShaderVariant::Default) const;
			std::shared_ptr<Texture>	getTexture(uint8_t _slot = 0) const;
			std::shared_ptr<Font>		getFont(uint8_t _slot = 0) const;
			bool						getLit() const;
			const std::string&			getName() const;

			void						setShader(std::shared_ptr<Shader> _shader, ShaderVariant variant = ShaderVariant::Default);
			void						setTexture(std::shared_ptr<Texture> _texture, std::string_view _uniformName, uint8_t _slot);
			void						setTexture(std::shared_ptr<Texture> _texture, uint8_t _slot = 0);
			void						setFont(std::shared_ptr<Font> _font, std::string_view _uniformName, uint8_t _slot);
			void						setFont(std::shared_ptr<Font> _font, uint8_t _slot = 0);
			void						setLit(bool _value);
			void						setName(std::string_view _name);

		private:

			struct MaterialTexture
			{
				std::unique_ptr<Uniform>	uniform;
				std::shared_ptr<Texture>	texture;
			};
			struct MaterialFont
			{
				std::unique_ptr<Uniform>	uniform;
				std::shared_ptr<Font>		font;
			};

			std::string name = "unknown";
			std::unordered_map<ShaderVariant, std::shared_ptr<Shader>>		shaders;
			std::unordered_map<uint8_t, std::unique_ptr<MaterialTexture>>	textures;
			std::unordered_map<uint8_t, std::unique_ptr<MaterialFont>>		fonts;
			std::vector<std::shared_ptr<UniformContainer>>					uniformContainers;
			bool lightsEnabled = false;
			boost::signals2::signal<void(void)> fontChangedSignal;
		};
	}
}
