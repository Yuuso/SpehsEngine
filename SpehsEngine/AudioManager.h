
#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>


typedef unsigned int ALuint;
typedef int ALsizei;
typedef int ALenum;


namespace spehs
{
	class AudioManager
	{
		struct WAVE
		{
			struct RIFFHeader
			{
				char chunkID[4];
				int32_t chunkSize;
				char format[4];
			};

			struct WAVEFormat
			{
				char subChunkID[4];
				int32_t subChunkSize;
				int16_t audioFormat;
				int16_t numChannels;
				int32_t sampleRate;
				int32_t byteRate;
				int16_t blockAlign;
				int16_t bitsPerSample;
			};

			struct WAVEData
			{
				char subChunkID[4];
				int32_t subChunkSize;
			};

			RIFFHeader riffHeader;
			WAVEFormat waveFormat;
			WAVEData waveData;
			unsigned char* data;
		};

		class AudioClip
		{
		public:
			ALuint buffer = 0;
			ALsizei size = 0;
			ALsizei freq = 0;
			ALenum format = 0;
		};

	public:
		enum class AudioFormat
		{
			Mono8bit = 0x1100,
			Mono16bit = 0x1101,
			Stereo8bit = 0x1102,
			Stereo16bit = 0x1103
		};

		static AudioManager* instance;
		AudioManager();
		~AudioManager();

		size_t loadWAVE(const std::string &_filepath);
		size_t loadData(const std::string& _identifier, const unsigned char* _data, const int _size, const int _frequency, const AudioFormat _format);

		//TODO: Better memory management
		void deleteAudio(const std::string &_filepath);
		void deleteAudio(const size_t &_hashID);

		void deleteAllAudio();

		AudioClip getAudioClip(const std::string _filepath);
		AudioClip getAudioClip(const size_t &_hashID);

	private:
		std::unordered_map<size_t, AudioClip> audioClips;
	};
}

