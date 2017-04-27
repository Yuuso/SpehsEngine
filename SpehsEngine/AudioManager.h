
#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <mutex>


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

	public:
		class AudioClip
		{
		public:
			bool ready = false;
			ALuint buffer = 0;
			ALsizei size = 0;
			ALsizei freq = 0;
			ALenum format = 0;
		};

		enum class AudioFormat
		{
			Mono8bit = 0x1100,
			Mono16bit = 0x1101,
			Stereo8bit = 0x1102,
			Stereo16bit = 0x1103
		};

		static void init();
		static void uninit();
		
		//Check if sound loading is ready. Returns 0 if not ready. Returns buffer ID if ready. If sound doesn't exist gives a warning.
		static ALuint isReady(const size_t& _hashID);

		//Starts loading in another thread
		static size_t load(const std::string& _filepath);
		static size_t loadWAVE(const std::string& _filepath);
		static size_t loadOGG(const std::string& _filepath);

		//Not loaded in the background
		static size_t loadData(const std::string& _identifier, const unsigned char* _data, const int _size, const int _frequency, const AudioFormat _format);

		//TODO: Better memory management?
		static void deleteAudio(const std::string& _filepath);
		static void deleteAudio(const size_t& _hashID);

		static void deleteAllAudio();

		static AudioClip getAudioClip(const std::string _filepath);
		static AudioClip getAudioClip(const size_t& _hashID);

	private:
		static void bgloadWAVE(const std::string& _filepath);
		static void bgloadOGG(const std::string& _filepath);
	};
}

