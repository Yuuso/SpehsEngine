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


			void setAnimations(std::shared_ptr<std::vector<Animation>> _animations);
			void start(const std::string_view _name, const time::Time _fade);
			void pause(const std::string_view _name);
			void stop(const std::string_view _name, const time::Time _fade);
			void stop(const time::Time _fade);
			void setSpeed(const float _value, const std::string_view _name);
			void setSpeed(const float _value);
			bool isActive(const std::string_view _name) const;

			void update();
			bool hasAnimations() const;
			bool isActive() const;
			bool isActive(const std::string& _nodeName) const;
			glm::mat4 getTransform(const std::string& _nodeName) const;

		private:

			struct ActiveAnimation
			{
				void start();
				void pause();
				void fadeIn(const time::Time _fade);
				void fadeOut(const time::Time _fade);
				float getWeight() const;
				bool update(const time::Time _deltaTime);

				std::string name;
				const Animation* animation = nullptr;
				time::Time animTimer;
				time::Time fadeInTimer;
				time::Time fadeInTime;
				time::Time fadeOutTimer;
				time::Time fadeOutTime;
				bool fadingOut = false;
				bool fadingIn = false;
				bool paused = false;
				float speed = 1.0f;
				float weight = 1.0f;
			};

			std::shared_ptr<std::vector<Animation>> animations;
			std::vector<ActiveAnimation> activeAnimations;
			time::DeltaTimeSystem deltaTimeSystem;
		};
	}
}
