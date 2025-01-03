#pragma once

#include "SpehsEngine/Core/DeltaTimeSystem.h"


namespace se
{
	namespace graphics
	{
		struct Animation;

		class Animator
		{
		public:

			Animator() = default;
			~Animator() = default;

			Animator(const Animator& _other) = delete;
			Animator& operator=(const Animator& _other) = delete;

			Animator(Animator&& _other) = delete;
			Animator& operator=(Animator&& _other) = delete;


			void stopAll(Time _fade = Time::zero);
			void pauseAll(Time _fade = Time::zero);
			void resumeAll(Time _fade = Time::zero);
			void setGlobalSpeed(float _value);
			void setDefaultLooping(bool _value);

			void start(std::string_view _name, Time _fade = Time::zero);
			void pause(std::string_view _name);
			void stop(std::string_view _name, Time _fade = Time::zero);
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
				void fadeIn(Time _fade);
				void fadeOut(Time _fade);
				bool update(Time _deltaTime);
				bool fadingIn() const;
				bool fadingOut() const;
				bool isFinished() const;
				float getWeight() const;

				std::string name;
				const Animation* animation = nullptr;
				Time fadeInTime;
				Time fadeOutTime;

				Time animTimer;
				Time fadeInTimer;
				Time fadeOutTimer;

				bool paused = false;
				bool loop /*= true*/;
				bool stopWhenFinished = false;
				float speed = 1.0f;
			};

			ActiveAnimation* getActiveAnimation(std::string_view _name);
			const ActiveAnimation* getActiveAnimation(std::string_view _name) const;
			Animation* getAnimation(std::string_view _name);
			const Animation* getAnimation(std::string_view _name) const;

			std::shared_ptr<std::vector<std::unique_ptr<Animation>>> animations;
			std::vector<std::unique_ptr<ActiveAnimation>> activeAnimations;
			DeltaTimeSystem deltaTimeSystem;
			float globalSpeed = 1.0f;
			bool defaultLooping = true;
		};
	}
}
