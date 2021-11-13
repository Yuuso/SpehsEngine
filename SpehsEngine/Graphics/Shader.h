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


			void reload(std::shared_ptr<ResourceLoader> _resourceLoader = nullptr) override;
			bool reloadable() const override;

		private:

			friend class PrimitiveInternal;
			friend class Batch;
			friend class ShaderManager;

			static std::shared_ptr<ResourceData> createResource(const std::string _vertexShaderPath, const std::string _fragmentShaderPath);
			static std::shared_ptr<ResourceData> createResourceFromHandles(ResourceHandle _vertexShaderHandle, ResourceHandle _fragmentShaderHandle);
			void destroy();
			void create(const std::string_view _vertexShaderPath, const std::string_view _fragmentShaderPath, std::shared_ptr<ResourceLoader> _resourceLoader);
			void create(ResourceHandle _vertexShaderHandle, ResourceHandle _fragmentShaderHandle);

			std::string vertexShaderPath;
			std::string fragmentShaderPath;
		};
	}
}
