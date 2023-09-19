#pragma once

namespace se::gfx::impl
{
	struct AnimationNode
	{
		std::vector<std::pair<float, glm::vec3>> positionKeys;
		std::vector<std::pair<float, glm::quat>> rotationKeys;
		std::vector<std::pair<float, glm::vec3>> scalingKeys;
	};

	struct Animation
	{
		std::string name;
		int numFrames = -1;
		float framesPerSeconds = -1.0f;

		std::unordered_map<std::string, AnimationNode> channels;
	};
}
