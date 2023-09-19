#pragma once

#include "SpehsEngine/Graphics/Animator.h"


namespace se::gfx
{
	struct Animation;
}

namespace se::gfx::impl
{
	class AnimatorInternal : public Animator
	{
	public:

		AnimatorInternal() = default;
		~AnimatorInternal() = default;

		AnimatorInternal(const AnimatorInternal& _other) = delete;
		AnimatorInternal& operator=(const AnimatorInternal& _other) = delete;

		AnimatorInternal(AnimatorInternal&& _other) = delete;
		AnimatorInternal& operator=(AnimatorInternal&& _other) = delete;


		void setAnimations(std::shared_ptr<std::vector<std::unique_ptr<Animation>>> _animations);
		void update();
		glm::mat4 getTransform(const std::string& _nodeName) const;
		bool IsNodePlaying(const std::string& _nodeName) const;
	};
}
