#pragma once

#include "SpehsEngine/Core/Asset.h"


namespace se::gfx
{
	class ModelAsset : public IAsset
	{
	public:

		~ModelAsset() = default;

		// Construct empty model
		ModelAsset(std::string_view _name);

		// Construct and model from file
		ModelAsset(std::string_view _name, AsyncTaskManager* _taskManager, std::string_view _path);

		bool isReloadable() const override;
		void reload(AsyncTaskManager* _taskManager) override;

		// Load model from file
		void load(AsyncTaskManager* _taskManager, std::string_view _path);

	private:

		std::string path;
	};
}
