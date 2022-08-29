#include "stdafx.h"
#include "SpehsEngine/Audio/Bus.h"

#include "SpehsEngine/Audio/Internal/GlobalBackend.h"
#include "soloud/soloud_bus.h"


namespace se
{
	namespace audio
	{
		Bus::Bus()
			: audio::Instance()
		{
			bus = new SoLoud::Bus;
		}
		Bus::~Bus()
		{
			destroyedSignal();
			delete bus;
		}

		void Bus::makeMasterBus()
		{
			handle = globalSoloud->play(*bus);
			applyAttributes();
			isMaster = true;
		}

		void Bus::connect(Bus& _bus)
		{
			handle = _bus.bus->play(*bus);
			applyAttributes();
		}
		void Bus::disconnect()
		{
			se_assert(!isMaster);
			bus->stop();
			handle = invalidAudioHandle;
		}

		int Bus::getVoiceCount()
		{
			return bus->getActiveVoiceCount();
		}

		boost::signals2::scoped_connection Bus::connectToDestroyedSignal(std::function<void()> _func)
		{
			return destroyedSignal.connect(_func);
		}
	}
}
