#pragma once

#include "SpehsEngine/Graphics/Internal/Resource.h"
#include "SpehsEngine/Graphics/ResourceData.h"
#include <memory>
#include <string>


namespace se
{
	namespace graphics
	{
		class ModelData : public Resource<MeshData>
		{
		public:

			ModelData() = delete;
			ModelData(const std::string_view _name);
			~ModelData();

			ModelData(const ModelData& _other) = delete;
			ModelData& operator=(const ModelData& _other) = delete;

			ModelData(ModelData&& _other) = delete;
			ModelData& operator=(ModelData&& _other) = delete;


			void reload(std::shared_ptr<ResourceLoader> _resourceLoader = nullptr) override;
			bool reloadable() const override;

		private:

			friend class ModelDataManager;
			friend class Model;

			static std::shared_ptr<ResourceData> createResource(const std::string _path);
			void destroy();
			void create(const std::string_view _path, std::shared_ptr<ResourceLoader> _resourceLoader);

			std::string path;
		};
	}
}
