
#pragma once

#include <glm/vec2.hpp>

#include <string>



//Have this as class the users audio source ?

namespace spehs
{
	namespace audio
	{
		class AudioSource
		{
		public:
			AudioSource();
			~AudioSource();

			void setSound(const size_t _hashID);
			void setSound(const std::string _filepath);

			void play(); //Playing from a specific point?
						//Force play??
			void pause();
			void stop();

			void setLooping(const bool _loop);

			bool isPlaying();
			bool getLooping();

		private:
			glm::vec2 position;
			glm::vec2 velocity;
			glm::vec2 direction;
			float pitch;
			float gain;
			float maxGain;
			float minGain;
			bool loop;

			//Priority

			//save buffer?
			//Source pointer?
			//

			//Timing/Sync?
		};
	}
}

