
#pragma once

#include <glm/vec4.hpp>


namespace spehs
{
	enum CommonColor
	{
		WHITE = 0xFFFFFF,
		BLACK = 0x000000,
		RED = 0xFF0000,
		ORANGE = 0xFF8000,
		YELLOW = 0xFFFF00,
		GREEN = 0x00FF00,
		CYAN = 0x00FFFF,
		BLUE = 0x0000FF,
		MAGENTA = 0xFF00FF
	};

	glm::vec4 commonColor(CommonColor _color)
	{
		switch (_color)
		{
		case WHITE:
			return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		case BLACK:
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		case RED:
			return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		case ORANGE:
			return glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
		case YELLOW:
			return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		case GREEN:
			return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		case CYAN:
			return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		case BLUE:
			return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		case MAGENTA:
			return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		default:
			console::error("Invalid CommonColor: " + std::to_string(_color));
			break;
		}
	}
}