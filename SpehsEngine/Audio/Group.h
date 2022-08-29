#pragma once

#include "SpehsEngine/Audio/Internal/Instance.h"


namespace se
{
	namespace audio
	{
		class Group : public Instance
		{
		public:

			Group();
			~Group();

			Group(const Group& _other) = delete;
			Group& operator=(const Group& _other) = delete;
			Group(Group&& _other) = delete;
			Group& operator=(Group&& _other) = delete;


			void addVoice(Instance& _instance);
			void clearVoices();


			void setPause(bool _value);

		protected:

			virtual bool isHandleValid() const override;

		private:

			void initVoiceGroup();
		};
	}
}
