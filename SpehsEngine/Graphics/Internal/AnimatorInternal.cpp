#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/AnimatorInternal.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"


namespace se
{
	namespace graphics
	{
		void AnimatorInternal::setAnimations(std::shared_ptr<std::vector<Animation>> _animations)
		{
			animations = _animations;
			if (!animations)
				return;

			for (size_t i = 0; i < activeAnimations.size(); /*i++*/)
			{
				activeAnimations[i].animation = getAnimation(activeAnimations[i].name);
				if (!activeAnimations[i].animation)
				{
					log::warning("Animation " + activeAnimations[i].name + " not found");
					activeAnimations.erase(activeAnimations.begin() + i);
					continue;
				}
				i++;
			}
		}


		void AnimatorInternal::update()
		{
			deltaTimeSystem.update();
			auto it = activeAnimations.begin();
			while (it != activeAnimations.end())
			{
				if (!it->update(deltaTimeSystem.deltaTime * globalSpeed))
				{
					it = activeAnimations.erase(it);
					continue;
				}
				it++;
			}
		}


		static glm::vec3 getPosition(const AnimationNode& _node, const float _time)
		{
			if (_node.positionKeys.size() == 1)
				return _node.positionKeys[0].second;

			for (size_t p = 0; p < _node.positionKeys.size() - 1; p++)
			{
				if (_time <= _node.positionKeys[p + 1].first)
				{
					const float delta = _node.positionKeys[p + 1].first - _node.positionKeys[p].first;
					const float timeSinceFirstKey = _time - _node.positionKeys[p].first;
					return glm::mix(_node.positionKeys[p].second, _node.positionKeys[p + 1].second, timeSinceFirstKey / delta);
				}
			}

			se_assert_m(false, "Invalid animation time!");
			return glm::vec3(0.0f);
		}
		static glm::quat getRotation(const AnimationNode& _node, const float _time)
		{
			if (_node.rotationKeys.size() == 1)
				return _node.rotationKeys[0].second;

			for (size_t r = 0; r < _node.rotationKeys.size() - 1; r++)
			{
				if (_time <= _node.rotationKeys[r + 1].first)
				{
					const float delta = _node.rotationKeys[r + 1].first - _node.rotationKeys[r].first;
					const float timeSinceFirstKey = _time - _node.rotationKeys[r].first;
					return glm::slerp(_node.rotationKeys[r].second, _node.rotationKeys[r + 1].second, timeSinceFirstKey / delta);
				}
			}

			se_assert_m(false, "Invalid animation time!");
			return glm::quat(glm::vec3(0.0f));
		}
		static glm::vec3 getScale(const AnimationNode& _node, const float _time)
		{
			if (_node.scalingKeys.size() == 1)
				return _node.scalingKeys[0].second;

			for (size_t s = 0; s < _node.scalingKeys.size() - 1; s++)
			{
				if (_time <= _node.scalingKeys[s + 1].first)
				{
					const float delta = _node.scalingKeys[s + 1].first - _node.scalingKeys[s].first;
					const float timeSinceFirstKey = _time - _node.scalingKeys[s].first;
					return glm::mix(_node.scalingKeys[s].second, _node.scalingKeys[s + 1].second, timeSinceFirstKey / delta);
				}
			}

			se_assert_m(false, "Invalid animation time!");
			return glm::vec3(1.0f);
		}

		glm::mat4 AnimatorInternal::getTransform(const std::string& _nodeName) const
		{
			float weight = 0.0f;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			for (auto&& activeAnimation : activeAnimations)
			{
				auto it = activeAnimation.animation->channels.find(_nodeName);
				if (it == activeAnimation.animation->channels.end())
					continue;

				const float timeInFrames = activeAnimation.animTimer.asSeconds() * activeAnimation.animation->framesPerSeconds;
				const float animTime = activeAnimation.loop
					? static_cast<float>(fmod(timeInFrames, activeAnimation.animation->numFrames))
					: glm::clamp(timeInFrames, 0.0f, static_cast<float>(activeAnimation.animation->numFrames));

				weight += activeAnimation.getWeight();
				position = glm::mix(position, getPosition(it->second, animTime), activeAnimation.getWeight() / weight);
				rotation = glm::slerp(rotation, getRotation(it->second, animTime), activeAnimation.getWeight() / weight);
				scale = glm::mix(scale, getScale(it->second, animTime), activeAnimation.getWeight() / weight);
			}

			return glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale);
		}

		bool AnimatorInternal::IsNodePlaying(const std::string& _nodeName) const
		{
			for (auto&& activeAnimation : activeAnimations)
			{
				if (activeAnimation.animation->channels.find(_nodeName) != activeAnimation.animation->channels.end())
					return true;
			}
			return false;
		}
	}
}
