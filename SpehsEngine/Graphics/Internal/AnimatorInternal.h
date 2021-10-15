#pragma once

#include "SpehsEngine/Graphics/Animator.h"
#include "SpehsEngine/Graphics/Internal/Animation.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include <memory>
#include <string>
#include <vector>


namespace se
{
	namespace graphics
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


			void setAnimations(std::shared_ptr<std::vector<Animation>> _animations);
			void update();
			glm::mat4 getTransform(const std::string& _nodeName) const;
			bool IsNodePlaying(const std::string& _nodeName) const;
		};
	}
}
