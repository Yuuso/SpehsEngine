#include "stdafx.h"
#include "SpehsEngine/Audio/Internal/Instance.h"

#include "SpehsEngine/Audio/Internal/GlobalBackend.h"


namespace se
{
	namespace audio
	{
		Instance::Instance()
		{
			se_assert_m(globalSoloud, "AudioEngine not initialized!");
		}

		void Instance::setVolume(float _volume, time::Time _fade)
		{
			volume = _volume;

			if (isHandleValid())
			{
				applyVolume(_fade);
			}
			else if (_fade > time::Time::zero)
			{
				log::warning("Cannot fade volume, instance is not valid!");
			}
		}
		void Instance::setPan(float _pan, time::Time _fade)
		{
			pan = _pan;

			if (isHandleValid())
			{
				applyPan(_fade);
			}
			else if (_fade > time::Time::zero)
			{
				log::warning("Cannot fade pan, instance is not valid!");
			}
		}

		void Instance::applyVolume(time::Time _fade)
		{
			se_assert(isHandleValid());
			if (_fade > time::Time::zero)
			{
				globalSoloud->fadeVolume(handle, volume, _fade.asSeconds<double>());
			}
			else
			{
				globalSoloud->setVolume(handle, volume);
			}
		}
		void Instance::applyPan(time::Time _fade)
		{
			se_assert(isHandleValid());
			if (_fade > time::Time::zero)
			{
				globalSoloud->fadePan(handle, pan, _fade.asSeconds<double>());
			}
			else
			{
				globalSoloud->setPan(handle, pan);
			}
		}


		float Instance::getAudibleVolume() const
		{
			return isHandleValid() ? globalSoloud->getOverallVolume(handle) : 0.0f;
		}
		float Instance::getVolume() const
		{
			return isHandleValid() ? globalSoloud->getVolume(handle) : volume;
		}
		float Instance::getPan() const
		{
			return isHandleValid() ? globalSoloud->getPan(handle) : pan;
		}

		void Instance::applyAttributes()
		{
			se_assert(isHandleValid());
			applyVolume();
			applyPan();
		}
		bool Instance::isHandleValid() const
		{
			if (!globalSoloud) return false;
			return handle != invalidAudioHandle && globalSoloud->isValidVoiceHandle(handle);
		}
	}
}
