#include "stdafx.h"
#include "SpehsEngine/Graphics/Animator.h"

#include "SpehsEngine/Graphics/Internal/Animation.h"


namespace se
{
	namespace graphics
	{
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
			fadeInTime = _fade;
			fadeInTimer = _fade;
		}
		void Animator::ActiveAnimation::fadeOut(const time::Time _fade)
		{
			fadeOutTime = _fade;
			fadeOutTimer = _fade;

			if (fadeOutTimer <= se::time::Time::zero)
			{
				// Set non-zero timer to stop the animation on the next update
				fadeOutTimer = se::time::fromNanoseconds(1.0);
			}
		}
		bool Animator::ActiveAnimation::fadingIn() const
		{
			return fadeInTimer > time::Time::zero;
		}
		bool Animator::ActiveAnimation::fadingOut() const
		{
			return fadeOutTimer > time::Time::zero;
		}
		bool Animator::ActiveAnimation::isFinished() const
		{
			if (loop)
				return false;
			if (speed < 0.0f)
				return animTimer <= se::time::Time::zero;
			const float timeInFrames = animTimer.asSeconds() * animation->framesPerSeconds;
			return timeInFrames >= static_cast<float>(animation->numFrames);
		}
		float Animator::ActiveAnimation::getWeight() const
		{
			float result = 1.0f;
			if (fadingOut())
				result *= fadeOutTimer.asMilliseconds() / fadeOutTime.asMilliseconds();
			if (fadingIn())
				result *= 1.0f - (fadeInTimer.asMilliseconds() / fadeInTime.asMilliseconds());
			return result;
		}
		bool Animator::ActiveAnimation::update(const time::Time _deltaTime)
		{
			if (paused)
				return true;

			animTimer += _deltaTime * speed;
			if (stopWhenFinished && isFinished())
				return false;

			if (fadingOut())
			{
				fadeOutTimer -= _deltaTime;
				if (!fadingOut())
					return false;
			}
			if (fadingIn())
			{
				fadeInTimer -= _deltaTime;
				if (fadeInTimer < se::time::Time::zero)
					fadeInTimer = se::time::Time::zero;
			}
			return true;
		}



		Animator::ActiveAnimation* Animator::getActiveAnimation(std::string_view _name)
		{
			auto it = std::find_if(
				activeAnimations.begin(), activeAnimations.end(),
				[&](const std::unique_ptr<ActiveAnimation>& _animation)
				{
					return _animation->name == _name;
				});
			if (it == activeAnimations.end())
				return nullptr;
			return it->get();
		}
		Animation* Animator::getAnimation(const std::string_view _name)
		{
			auto it = std::find_if(
				animations->begin(), animations->end(),
				[&](const std::unique_ptr<Animation>& _animation)
				{
					return _animation->name == _name;
				});
			if (it == animations->end())
				return nullptr;
			return it->get();
		}
		const Animator::ActiveAnimation* Animator::getActiveAnimation(std::string_view _name) const
		{
			auto it = std::find_if(
				activeAnimations.begin(), activeAnimations.end(),
				[&](const std::unique_ptr<ActiveAnimation>& _animation)
				{
					return _animation->name == _name;
				});
			if (it == activeAnimations.end())
				return nullptr;
			return it->get();
		}
		const Animation* Animator::getAnimation(const std::string_view _name) const
		{
			auto it = std::find_if(
				animations->begin(), animations->end(),
				[&](const std::unique_ptr<Animation>& _animation)
				{
					return _animation->name == _name;
				});
			if (it == animations->end())
				return nullptr;
			return it->get();
		}


		void Animator::stopAll(time::Time _fade)
		{
			for (auto&& anim : activeAnimations)
				anim->fadeOut(_fade);
		}
		void Animator::pauseAll(time::Time _fade)
		{
			for (auto&& anim : activeAnimations)
				anim->pause();
		}
		void Animator::resumeAll(time::Time _fade)
		{
			for (auto&& anim : activeAnimations)
			{
				if (anim->paused)
					anim->start();
			}
		}
		void Animator::setGlobalSpeed(float _value)
		{
			globalSpeed = _value;
		}
		void Animator::setDefaultLooping(bool _value)
		{
			defaultLooping = _value;
		}

		void Animator::start(std::string_view _name, time::Time _fade)
		{
			// Restart / unpause an already active animation
			if (ActiveAnimation* anim = getActiveAnimation(_name))
			{
				anim->start();
				anim->fadeIn(_fade);
				return;
			}

			// Start new animation
			if (animations)
			{
				if (Animation* anim = getAnimation(_name))
				{
					activeAnimations.push_back(std::make_unique<ActiveAnimation>(defaultLooping));
					ActiveAnimation& activeAnimation = *activeAnimations.back();
					activeAnimation.name = _name;
					activeAnimation.animation = anim;
					activeAnimation.start();
					activeAnimation.fadeIn(_fade);
					return;
				}
			}
			else
			{
				// Animations not loaded yet
				activeAnimations.push_back(std::make_unique<ActiveAnimation>(defaultLooping));
				ActiveAnimation& activeAnimation = *activeAnimations.back();
				activeAnimation.name = _name;
				activeAnimation.animation = nullptr;
				activeAnimation.start();
				activeAnimation.fadeIn(_fade);
				return;
			}
			log::warning("Animation " + _name + " not found! Cannot start animation.");
		}
		void Animator::pause(std::string_view _name)
		{
			if (ActiveAnimation* anim = getActiveAnimation(_name))
			{
				anim->pause();
				return;
			}
			log::warning("Animation '" + _name + "' not found! Cannot pause animation.");
		}
		void Animator::stop(std::string_view _name, time::Time _fade)
		{
			if (ActiveAnimation* anim = getActiveAnimation(_name))
			{
				anim->fadeOut(_fade);
				return;
			}
			log::warning("Animation '" + _name + "' not found! Cannot stop animation.");
		}
		void Animator::stopWhenFinished(std::string_view _name, bool _value)
		{
			if (ActiveAnimation* anim = getActiveAnimation(_name))
			{
				anim->stopWhenFinished = _value;
				return;
			}
			log::warning("Animation '" + _name + "' not found! Cannot enable stopWhenFinished.");
		}
		void Animator::setSpeed(std::string_view _name, float _value)
		{
			if (ActiveAnimation* anim = getActiveAnimation(_name))
			{
				anim->speed = _value;
				return;
			}
			log::warning("Animation '" + _name + "' not found! Cannot set animation speed.");
		}
		void Animator::setLooping(std::string_view _name, bool _value)
		{
			if (ActiveAnimation* anim = getActiveAnimation(_name))
			{
				anim->loop = _value;
				return;
			}
			log::warning("Animation '" + _name + "' not found! Cannot set animation looping.");
		}

		bool Animator::isFinished(std::string_view _name) const
		{
			if (const ActiveAnimation* anim = getActiveAnimation(_name))
			{
				return anim->isFinished();
			}
			log::warning("Animation '" + _name + "' not found!");
			return true;
		}
		bool Animator::isPlaying() const
		{
			return !activeAnimations.empty();
		}
		bool Animator::isPlaying(std::string_view _name) const
		{
			return getActiveAnimation(_name) != nullptr;
		}
		bool Animator::hasAnimations() const
		{
			return animations != nullptr && !animations->empty();
		}
		bool Animator::hasAnimation(std::string_view _name) const
		{
			return animations != nullptr && getAnimation(_name) != nullptr;
		}
	}
}
