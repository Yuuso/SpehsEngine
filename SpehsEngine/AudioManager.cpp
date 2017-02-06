
#include "AudioManager.h"
#include "Exceptions.h"
#include "OpenALError.h"

#include <AL\al.h>

#include <functional>


namespace spehs
{
	AudioManager* AudioManager::instance = nullptr;
	AudioManager::AudioManager()
	{

	}
	AudioManager::~AudioManager()
	{
		deleteAllAudio();
	}

	size_t AudioManager::loadWAVE(const std::string &_filepath)
	{
		size_t hash = std::hash<std::string>()(_filepath);
		if (audioClips.find(hash) != audioClips.end())
			return hash;

		FILE* fileData = nullptr;
		WAVE waveFile;
		AudioClip clip;

		//Open file and make visual studio happy by doind it safely...
#ifdef WIN32
		errno_t error = fopen_s(&fileData, _filepath.c_str(), "rb");
		if (error != 0)
		{
			exceptions::fatalError("Failed to open WAVE file, error: " + std::to_string(error));
		}
#else
		fileData = fopen(_filepath.c_str(), "rb");
		if (!fileData)
		{
			exceptions::fatalError("Failed to open WAVE file!");
		}
#endif

		//Read RIFF header
		fread(&waveFile.riffHeader, sizeof(WAVE::RIFFHeader), 1, fileData);

		//Check for correct tags
		if ((waveFile.riffHeader.chunkID[0] != 'R' ||
			waveFile.riffHeader.chunkID[1] != 'I' ||
			waveFile.riffHeader.chunkID[2] != 'F' ||
			waveFile.riffHeader.chunkID[3] != 'F') ||
			(waveFile.riffHeader.format[0] != 'W' ||
			waveFile.riffHeader.format[1] != 'A' ||
			waveFile.riffHeader.format[2] != 'V' ||
			waveFile.riffHeader.format[3] != 'E'))
		{
			fclose(fileData);
			exceptions::fatalError("Invalid RIFF or WAVE header!");
		}

		//Read format
		fread(&waveFile.waveFormat, sizeof(WAVE::WAVEFormat), 1, fileData);

		//Check for correct tag
		if (waveFile.waveFormat.subChunkID[0] != 'f' ||
			waveFile.waveFormat.subChunkID[1] != 'm' ||
			waveFile.waveFormat.subChunkID[2] != 't' ||
			waveFile.waveFormat.subChunkID[3] != ' ')
		{
			fclose(fileData);
			exceptions::fatalError("Invalid Wave format!");
		}

		//Check for extra parameters
		if (waveFile.waveFormat.subChunkSize > 16)
		{
			fseek(fileData, sizeof(uint16_t), SEEK_CUR);
		}

		//The format is worked out by looking at the number of channels and the bits per sample
		if (waveFile.waveFormat.numChannels == 1)
		{
			if (waveFile.waveFormat.bitsPerSample == 8)
				clip.format = AL_FORMAT_MONO8;
			else if (waveFile.waveFormat.bitsPerSample == 16)
				clip.format = AL_FORMAT_MONO16;
			else
			{
				fclose(fileData);
				exceptions::fatalError("Bit depth of loaded mono audio file not supported! (File name: " + _filepath + " )");
			}
		}
		else if (waveFile.waveFormat.numChannels == 2)
		{
			if (waveFile.waveFormat.bitsPerSample == 8)
				clip.format = AL_FORMAT_STEREO8;
			else if (waveFile.waveFormat.bitsPerSample == 16)
				clip.format = AL_FORMAT_STEREO16;
			else
			{
				fclose(fileData);
				exceptions::fatalError("Bit depth of loaded stereo audio file not supported! (File name : " + _filepath + ")");
			}
		}
		else
		{
			fclose(fileData);
			exceptions::fatalError("Invalid number of channels! (File name : " + _filepath + ")");
		}

		//Read WAVE data
		fread(&waveFile.waveData, sizeof(WAVE::WAVEData), 1, fileData);

		//Check for correct data tag
		if (waveFile.waveData.subChunkID[0] != 'd' ||
			waveFile.waveData.subChunkID[1] != 'a' ||
			waveFile.waveData.subChunkID[2] != 't' ||
			waveFile.waveData.subChunkID[3] != 'a')
		{
			fclose(fileData);
			exceptions::fatalError("Invalid data tag!");
		}

		//Set variables
		clip.size = waveFile.waveData.subChunkSize;
		clip.freq = waveFile.waveFormat.sampleRate;

		//Read sound data
		waveFile.data = new unsigned char[waveFile.waveData.subChunkSize];
		if (!fread(waveFile.data, waveFile.waveData.subChunkSize, 1, fileData))
		{
			fclose(fileData);
			exceptions::fatalError("Error loading WAVE sound file!");
		}

		//Generate OpenAL buffer
		alGenBuffers(1, &clip.buffer);
		checkOpenALErrors(__FILE__, __LINE__);

		//Data into the buffer
		alBufferData(clip.buffer, clip.format, (void*) waveFile.data, clip.size, clip.freq);
		checkOpenALErrors(__FILE__, __LINE__);

		//Clean ups
		fclose(fileData);
		delete[] waveFile.data;
		
		audioClips.insert(std::pair<size_t, AudioClip>(hash, clip));
		return hash;
	}
	size_t AudioManager::loadData(const std::string& _identifier, const unsigned char* _data, const int _size, const int _frequency, const AudioFormat _format)
	{
		size_t hash = std::hash<std::string>()(_identifier);
		if (audioClips.find(hash) != audioClips.end())
			return hash;

		AudioClip clip;

		clip.format = (ALenum)_format;
		clip.freq = (ALsizei)_frequency;
		clip.size = (ALsizei)_size;
		
		//Generate OpenAL buffer
		alGenBuffers(1, &clip.buffer);
		checkOpenALErrors(__FILE__, __LINE__);

		//Data into the buffer
		alBufferData(clip.buffer, clip.format, (void*) _data, clip.size, clip.freq);
		checkOpenALErrors(__FILE__, __LINE__);

		audioClips.insert(std::pair<size_t, AudioClip>(hash, clip));
		return hash;
	}

	void AudioManager::deleteAudio(const std::string &_filepath)
	{
		deleteAudio(std::hash<std::string>()(_filepath));
	}
	void AudioManager::deleteAudio(const size_t &_hashID)
	{
		auto it = audioClips.find(_hashID);
		if (it == audioClips.end())
		{
			exceptions::fatalError("Trying to delete a WAVE file that doesn't exist!");
		}

		alDeleteBuffers(1, &it->second.buffer);

		audioClips.erase(_hashID);
	}
	void AudioManager::deleteAllAudio()
	{
		for (auto &it : audioClips)
		{
			alDeleteBuffers(1, &it.second.buffer);
		}
		audioClips.clear();
	}

	AudioManager::AudioClip AudioManager::getAudioClip(const std::string _filepath)
	{
		return getAudioClip(std::hash<std::string>()(_filepath));
	}
	AudioManager::AudioClip AudioManager::getAudioClip(const size_t &_hashID)
	{
		auto it = audioClips.find(_hashID);
		if (it == audioClips.end())
		{
			exceptions::fatalError("Trying to access non existent audio clip!");
		}

		return it->second;
	}
}