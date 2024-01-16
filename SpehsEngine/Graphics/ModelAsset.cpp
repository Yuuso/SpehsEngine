#include "stdafx.h"
#include "SpehsEngine/Graphics/ModelAsset.h"

#include "SpehsEngine/Graphics/Impl/ModelLoad.h"


ModelAsset::ModelAsset(std::string_view _name)
	: IAsset(_name)
{}
ModelAsset::ModelAsset(std::string_view _name, AsyncTaskManager* _taskManager, std::string_view _path)
	: ModelAsset(_name)
{
	load(_taskManager, _path);
}
bool ModelAsset::isReloadable() const
{
	return !path.empty();
}
void ModelAsset::reload(AsyncTaskManager* _taskManager)
{
	if (!isReloadable())
	{
		log::warning("ModelAsset " + getName() + " not reloadable!");
		return;
	}
	load(_taskManager, path);
}
void ModelAsset::load(AsyncTaskManager* _taskManager, std::string_view _path)
{
	path = _path;
	IAsset::load(_taskManager, [this]{ return loadModel(path); });
}
