#include "stdafx.h"
#include "SpehsEngine/Audio/Group.h"

#include "SpehsEngine/Audio/Internal/GlobalBackend.h"


namespace se
{
	namespace audio
	{
		Group::Group()
			: Instance()
		{
			initVoiceGroup();
		}
		Group::~Group()
		{
			globalSoloud->destroyVoiceGroup(handle);
		}
		void Group::initVoiceGroup()
		{
			handle = globalSoloud->createVoiceGroup();
			if (!isHandleValid())
			{
				se::log::error("Failed to create a voice group!");
			}
		}
		void Group::addVoice(Instance& _instance)
		{
			globalSoloud->addVoiceToGroup(handle, _instance.handle);
		}
		void Group::clearVoices()
		{
			globalSoloud->destroyVoiceGroup(handle);
			initVoiceGroup();
		}
		void Group::setPause(bool _value)
		{
			se_assert(isHandleValid());
			globalSoloud->setPause(handle, _value);
		}
		bool Group::isHandleValid() const
		{
			return globalSoloud->isVoiceGroup(handle);
		}
	}
}
