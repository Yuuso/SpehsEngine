#pragma once

#include "SpehsEngine/Graphics/ShaderVariant.h"

namespace se::gfx
{
	class Font;
	class Shader;
	class Texture;
	class Uniform;
}

namespace se::gfx
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

		SE_NO_COPY_OR_MOVE(Material);
											Material() = default;
											~Material() = default;


		void								bind(Texture* _fallbackTexture);
		boost::signals2::scoped_connection	connectToFontChangedSignal(const boost::function<void()>& callback);

		std::shared_ptr<const Shader>		getShader(ShaderVariant variant = ShaderVariant::Default) const;
		std::shared_ptr<const Texture>		getTexture(uint8_t _slot = 0) const;
		std::shared_ptr<const Font>			getFont(uint8_t _slot = 0) const;
		bool								checkUniformContainer(const std::string& _name) const;
		bool								getLit() const;
		const std::string&					getName() const;

		void								setShader(std::shared_ptr<Shader> _shader, ShaderVariant variant = ShaderVariant::Default);
		void								setTexture(std::shared_ptr<Texture> _texture, std::string_view _uniformName, uint8_t _slot);
		void								setTexture(std::shared_ptr<Texture> _texture, uint8_t _slot = 0);
		void								setFont(std::shared_ptr<Font> _font, std::string_view _uniformName, uint8_t _slot);
		void								setFont(std::shared_ptr<Font> _font, uint8_t _slot = 0);
		void								setUniformContainer(std::shared_ptr<UniformContainer> _uniformContainer, const std::string& _name);
		void								setLit(bool _value);
		void								setName(std::string_view _name);

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

		std::unordered_map<ShaderVariant, std::shared_ptr<Shader>>			shaders;
		std::unordered_map<uint8_t, std::unique_ptr<MaterialTexture>>		textures;
		std::unordered_map<uint8_t, std::unique_ptr<MaterialFont>>			fonts;
		std::unordered_map<std::string, std::shared_ptr<UniformContainer>>	uniformContainers;

		std::string name = "unknown";
		bool lightsEnabled = false;
		boost::signals2::signal<void()> fontChangedSignal;
	};
}
