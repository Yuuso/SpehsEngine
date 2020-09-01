#pragma once

#include "SpehsEngine/Graphics/Internal/Resource.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include "SpehsEngine/Graphics/ResourceData.h"
#include <string>


namespace se
{
	namespace graphics
	{
		class Shader : public Resource<ShaderData>
		{
		public:

			Shader() = delete;
			Shader(const std::string_view _name);
			~Shader();

			Shader(const Shader& _other) = delete;
			Shader& operator=(const Shader& _other) = delete;

			Shader(Shader&& _other) = delete;
			Shader& operator=(Shader&& _other) = delete;


			void reload(ResourceLoader _resourceLoader = nullptr) override;

			const std::string& getName() const;

		private:

			friend class PrimitiveInstance;
			friend class Batch;
			friend class ShaderManager;
			friend class DefaultShaderManager;

			static std::shared_ptr<ResourceData> createResource(const std::string _vertexShaderPath, const std::string _fragmentShaderPath);
			static std::shared_ptr<ResourceData> createResourceFromHandles(ResourceHandle _vertexShaderHandle, ResourceHandle _fragmentShaderHandle);
			void destroy();
			void create(const std::string_view _vertexShaderPath, const std::string_view _fragmentShaderPath, ResourceLoader _resourceLoader);
			void create(ResourceHandle _vertexShaderHandle, ResourceHandle _fragmentShaderHandle);
			void extractUniforms(std::vector<std::shared_ptr<Uniform>>& _uniforms);

			const std::string name;

			std::string vertexShaderPath;
			std::string fragmentShaderPath;
		};
	}
}
