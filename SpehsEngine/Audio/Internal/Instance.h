#pragma once

#include "SpehsEngine/Audio/Internal/InternalTypes.h"


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
			void setVolume(float _volume, Time _fade = Time::zero);

			/// Pan setting, -1.0: left, 0.0: middle, 1.0: right
			void setPan(float _pan, Time _fade = Time::zero);


			float getAudibleVolume() const;
			float getVolume() const;
			float getPan() const;

		protected:

			friend class Group;

			virtual void applyAttributes();
			virtual bool isHandleValid() const;

			AudioHandle handle = invalidAudioHandle;

		private:

			void applyVolume(Time _fade = Time::zero);
			void applyPan(Time _fade = Time::zero);

			float volume = 1.0f;
			float pan = 0.0f;
		};
	}
}
