#include "stdafx.h"
#include "SpehsEngine/Debug/ImGfx.h"

using namespace se::debug;


namespace ImGfx
{
	static ImmediateModeGraphics* instance = nullptr;

	void init(ImmediateModeGraphics& _instance)
	{
		instance = &_instance;
	}
	void deinit()
	{
		instance = nullptr;
	}

	PrimitiveModifier shape(se::gfx::ShapeType _type, se::gfx::ShapeParameters _params)
	{
		if (!instance) se::log::fatal("ImGfx not initialized!");
		return instance->shape(_type, _params);
	}
	PrimitiveModifier sphere()
	{
		return shape(se::gfx::ShapeType::Sphere);
	}
	PrimitiveModifier box()
	{
		return shape(se::gfx::ShapeType::Box);
	}
	PrimitiveModifier icon(std::string_view _texture)
	{
		if (!instance) se::log::fatal("ImGfx not initialized!");
		return instance->icon(_texture);
	}
	PrimitiveModifier line(const glm::vec3& _start, const glm::vec3& _end)
	{
		if (!instance) se::log::fatal("ImGfx not initialized!");
		return instance->line(_start, _end);
	}
	PrimitiveModifier linePoint(const glm::vec3& _point, std::string_view _lineId)
	{
		if (!instance) se::log::fatal("ImGfx not initialized!");
		return instance->linePoint(_point, _lineId);
	}
	PrimitiveModifier text(const std::string& _text, std::string_view _font)
	{
		if (!instance) se::log::fatal("ImGfx not initialized!");
		return instance->text(_text, _font);
	}
	ModelModifier model(std::string_view _model)
	{
		if (!instance) se::log::fatal("ImGfx not initialized!");
		return instance->model(_model);
	}
}
