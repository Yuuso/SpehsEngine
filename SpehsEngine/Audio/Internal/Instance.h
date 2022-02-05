#pragma once

#include "SpehsEngine/Audio/Internal/InternalTypes.h"
#include "SpehsEngine/Core/SE_Time.h"


namespace se
{
	namespace audio
	{
		class Instance
		{
		public:

			Instance();
			virtual ~Instance() = default;

			Instance(const Instance& _other) = delete;
			Instance& operator=(const Instance& _other) = delete;
			Instance(Instance&& _other) = delete;
			Instance& operator=(Instance&& _other) = delete;


			/// Volume setting, 0.0 - ?
			void setVolume(float _volume, time::Time _fade = time::Time::zero);

			/// Pan setting, -1.0: left, 0.0: middle, 1.0: right
			void setPan(float _pan, time::Time _fade = time::Time::zero);


			float getAudibleVolume() const;
			float getVolume() const;
			float getPan() const;

		protected:

			virtual void applyAttributes();
			bool isHandleValid() const;

			AudioHandle handle = 0;

		private:

			void applyVolume(time::Time _fade = time::Time::zero);
			void applyPan(time::Time _fade = time::Time::zero);

			float volume = 1.0f;
			float pan = 0.0f;
		};
	}
}
