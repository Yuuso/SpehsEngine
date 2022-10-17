#pragma once

#include "SpehsEngine/Debug/ImmediateModeGraphics.h"


namespace ImGfx
{
	void init(se::debug::ImmediateModeGraphics& _instance);
	void deinit();

	se::debug::PrimitiveModifier	shape(se::graphics::ShapeType _type, se::graphics::ShapeParameters _params = {});
	se::debug::PrimitiveModifier	sphere();
	se::debug::PrimitiveModifier	box();
	se::debug::PrimitiveModifier	icon(std::string_view _texture);
	se::debug::PrimitiveModifier	line(const glm::vec3& _start, const glm::vec3& _end);
	se::debug::PrimitiveModifier	linePoint(const glm::vec3& _point, std::string_view _lineId);
	se::debug::PrimitiveModifier	text(const std::string& _text, std::string_view _font = "");
	se::debug::ModelModifier		model(std::string_view _model);
}
