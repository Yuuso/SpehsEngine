#pragma once

#include "SpehsEngine/Audio/Internal/Instance.h"
#include "SpehsEngine/Audio/Internal/InternalTypes.h"


namespace se
{
	namespace audio
	{
		class Bus final : public audio::Instance
		{
		public:

			Bus();
			~Bus();

			Bus(const Bus& _other) = delete;
			Bus& operator=(const Bus& _other) = delete;
			Bus(Bus&& _other) = delete;
			Bus& operator=(Bus&& _other) = delete;


			void connect(Bus& _bus);
			void disconnect();

			int getVoiceCount();

			[[nodiscard]] boost::signals2::scoped_connection connectToDestroyedSignal(std::function<void()> _func);

		private:

			friend class AudioEngine;
			friend class AudioSource;

			void makeMasterBus();

			bool isMaster = false;
			AudioBus* bus = nullptr;
			boost::signals2::signal<void()> destroyedSignal;
		};
	}
}
