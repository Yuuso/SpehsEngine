#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/Animator.h"

#include "SpehsEngine/Core/StringViewUtilityFunctions.h"

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
		time::Time(*getAnimationRefTime)(void) = time::getRunTime;

		void Animator::ActiveAnimation::start()
		{
			if (paused)
			{
				paused = false;
			}
			else
			{
				animTimer = time::Time::zero;
			}
		}
		void Animator::ActiveAnimation::pause()
		{
			paused = true;
		}
		void Animator::ActiveAnimation::fadeIn(const time::Time _fade)
		{
			fadingOut = false;
			fadingIn = true;
			fadeInTime = _fade;
			fadeInTimer = _fade;
		}
		void Animator::ActiveAnimation::fadeOut(const time::Time _fade)
		{
			if (fadingOut)
				return;
			fadingOut = true;
			fadeOutTime = _fade;
			fadeOutTimer = _fade;
		}
		float Animator::ActiveAnimation::getWeight() const
		{
			float result = weight;
			if (fadingOut)
				result *= fadeOutTimer.asMilliseconds() / fadeOutTime.asMilliseconds();
			if (fadingIn)
				result *= 1.0f - (fadeInTimer.asMilliseconds() / fadeInTime.asMilliseconds());
			return result;
		}
		bool Animator::ActiveAnimation::update(const time::Time _deltaTime)
		{
			if (paused)
				return true;
			animTimer += _deltaTime;
			if (fadingOut)
			{
				fadeOutTimer -= _deltaTime;
				if (fadeOutTimer < time::Time::zero)
					return false;
			}
			if (fadingIn)
			{
				fadeInTimer -= _deltaTime;
				if (fadeInTimer < time::Time::zero)
					fadingIn = false;
			}
			return true;
		}


		void Animator::setAnimations(std::shared_ptr<std::vector<Animation>> _animations)
		{
			animations = _animations;
			if (!animations)
				return;

			for (size_t i = 0; i < activeAnimations.size(); /*i++*/)
			{
				auto it = std::find_if(
					animations->begin(), animations->end(),
					[&](const Animation& _animation)
					{
						return _animation.name == activeAnimations[i].name;
					});
				if (it == animations->end())
				{
					log::warning("Animation " + activeAnimations[i].name + " not found");
					activeAnimations.erase(activeAnimations.begin() + i);
				}
				else
				{
					activeAnimations[i].animation = &*it;
					i++;
				}
			}
		}
		void Animator::start(const std::string_view _name, const time::Time _fade)
		{
			// Restart / unpause an already active animation
			for (auto&& animation : activeAnimations)
			{
				if (animation.name != _name)
					continue;

				animation.start();
				animation.fadeIn(_fade);
				return;
			}

			// Start new animation
			if (animations)
			{
				auto it = std::find_if(
					animations->begin(), animations->end(),
					[&_name](const Animation& _animation)
					{
						return _animation.name == _name;
					});
				if (it != animations->end())
				{
					activeAnimations.push_back(ActiveAnimation());
					ActiveAnimation& activeAnimation = activeAnimations.back();
					activeAnimation.name = _name;
					activeAnimation.animation = &*it;
					activeAnimation.start();
					activeAnimation.fadeIn(_fade);
					return;
				}
			}
			else
			{
				// Animations not loaded yet
				activeAnimations.push_back(ActiveAnimation());
				ActiveAnimation& activeAnimation = activeAnimations.back();
				activeAnimation.name = _name;
				activeAnimation.start();
				activeAnimation.fadeIn(_fade);
				return;
			}

			log::warning("Animation " + _name + " not found");
		}
		void Animator::pause(const std::string_view _name)
		{
			auto it = std::find_if(
				activeAnimations.begin(), activeAnimations.end(),
				[&_name](const ActiveAnimation& _animation)
				{
					return _animation.name == _name;
				});
			if (it != activeAnimations.end())
			{
				it->pause();
			}
			else
			{
				log::warning("Cannot set animation speed, '" + _name + "' not found!");
			}
		}
		void Animator::stop(const std::string_view _name, const time::Time _fade)
		{
			for (auto&& activeAnimation : activeAnimations)
			{
				if (activeAnimation.name == _name)
				{
					activeAnimation.fadeOut(_fade);
					return;
				}
			}
		}
		void Animator::stop(const time::Time _fade)
		{
			for (auto&& activeAnimation : activeAnimations)
			{
				activeAnimation.fadeOut(_fade);
			}
		}
		void Animator::setSpeed(const float _value, const std::string_view _name)
		{
			auto it = std::find_if(
				activeAnimations.begin(), activeAnimations.end(),
				[&_name](const ActiveAnimation& _animation)
				{
					return _animation.name == _name;
				});
			if (it != activeAnimations.end())
			{
				it->speed = _value;
			}
			else
			{
				log::warning("Cannot set animation speed, '" + _name + "' not found!");
			}
		}
		void Animator::setSpeed(const float _value)
		{
			for (auto&& activeAnimation : activeAnimations)
			{
				activeAnimation.speed = _value;
			}
		}
		bool Animator::isActive(const std::string_view _name) const
		{
			return std::find_if(activeAnimations.begin(), activeAnimations.end(), [&_name](const ActiveAnimation& _anim) { return _name == _anim.name; }) != activeAnimations.end();
		}


		void Animator::update()
		{
			deltaTimeSystem.update();
			for (size_t i = 0; i < activeAnimations.size(); /*i++*/)
			{
				if (!activeAnimations[i].update(deltaTimeSystem.deltaTime))
				{
					activeAnimations.erase(activeAnimations.begin() + i);
				}
				else
				{
					i++;
				}
			}
		}
		bool Animator::hasAnimations() const
		{
			return animations != nullptr && !animations->empty();
		}
		bool Animator::isActive() const
		{
			return !activeAnimations.empty();
		}
		bool Animator::isActive(const std::string& _nodeName) const
		{
			for (auto&& activeAnimation : activeAnimations)
			{
				if (activeAnimation.animation->channels.find(_nodeName) != activeAnimation.animation->channels.end())
					return true;
			}
			return false;
		}


		static glm::vec3 getPosition(const AnimationNode& _node, const float _time)
		{
			if (_node.positionKeys.size() == 1)
				return _node.positionKeys[0].second;

			for (size_t p = 0; p < _node.positionKeys.size() - 1; p++)
			{
				if (_time < _node.positionKeys[p + 1].first)
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
				if (_time < _node.rotationKeys[r + 1].first)
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
				if (_time < _node.scalingKeys[s + 1].first)
				{
					const float delta = _node.scalingKeys[s + 1].first - _node.scalingKeys[s].first;
					const float timeSinceFirstKey = _time - _node.scalingKeys[s].first;
					return glm::mix(_node.scalingKeys[s].second, _node.scalingKeys[s + 1].second, timeSinceFirstKey / delta);
				}
			}

			se_assert_m(false, "Invalid animation time!");
			return glm::vec3(1.0f);
		}

		glm::mat4 Animator::getTransform(const std::string& _name) const
		{
			float weight = 0.0f;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			// NOTE: Not sure how this works with 3 or more animations on a single node

			for (auto&& activeAnimation : activeAnimations)
			{
				auto it = activeAnimation.animation->channels.find(_name);
				if (it == activeAnimation.animation->channels.end())
					continue;

				const float timeInSeconds = activeAnimation.animTimer.asSeconds() * activeAnimation.speed;
				const float timeInFrames = timeInSeconds * activeAnimation.animation->framesPerSeconds;
				const float animTime = static_cast<float>(fmod(timeInFrames, activeAnimation.animation->numFrames));

				weight += activeAnimation.getWeight();
				position = glm::mix(position, getPosition(it->second, animTime), activeAnimation.getWeight() / weight);
				rotation = glm::slerp(rotation, getRotation(it->second, animTime), activeAnimation.getWeight() / weight);
				scale = glm::mix(scale, getScale(it->second, animTime), activeAnimation.getWeight() / weight);
			}

			return glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale);
		}
	}
}
