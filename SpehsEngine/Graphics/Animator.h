#pragma once

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
		class Animator
		{
		public:

			Animator() = default;
			~Animator() = default;

			Animator(const Animator& _other) = delete;
			Animator& operator=(const Animator& _other) = delete;

			Animator(Animator&& _other) = delete;
			Animator& operator=(Animator&& _other) = delete;


			void stopAll(time::Time _fade = se::time::Time::zero);
			void pauseAll(time::Time _fade = se::time::Time::zero);
			void resumeAll(time::Time _fade = se::time::Time::zero);
			void setGlobalSpeed(float _value);
			void setDefaultLooping(bool _value);

			void start(std::string_view _name, time::Time _fade = se::time::Time::zero);
			void pause(std::string_view _name);
			void stop(std::string_view _name, time::Time _fade = se::time::Time::zero);
			void stopWhenFinished(std::string_view _name, bool _value);
			void setSpeed(std::string_view _name, float _value);
			void setLooping(std::string_view _name, bool _value);

			bool isFinished(std::string_view _name) const;
			bool isPlaying() const;
			bool isPlaying(std::string_view _name) const;
			bool hasAnimations() const;
			bool hasAnimation(std::string_view _name) const;

		protected:

			struct ActiveAnimation
			{
				ActiveAnimation() = delete;
				ActiveAnimation(bool _loop)
					: loop(_loop) {}

				void start();
				void pause();
				void fadeIn(time::Time _fade);
				void fadeOut(time::Time _fade);
				bool update(time::Time _deltaTime);
				bool fadingIn() const;
				bool fadingOut() const;
				bool isFinished() const;
				float getWeight() const;

				std::string name;
				const Animation* animation = nullptr;
				time::Time fadeInTime;
				time::Time fadeOutTime;

				time::Time animTimer;
				time::Time fadeInTimer;
				time::Time fadeOutTimer;

				bool paused = false;
				bool loop /*= true*/;
				bool stopWhenFinished = false;
				float speed = 1.0f;
			};

			ActiveAnimation* getActiveAnimation(std::string_view _name);
			const ActiveAnimation* getActiveAnimation(std::string_view _name) const;
			Animation* getAnimation(std::string_view _name);
			const Animation* getAnimation(std::string_view _name) const;

			std::shared_ptr<std::vector<Animation>> animations;
			std::vector<ActiveAnimation> activeAnimations;
			time::DeltaTimeSystem deltaTimeSystem;
			float globalSpeed = 1.0f;
			bool defaultLooping = true;
		};
	}
}
