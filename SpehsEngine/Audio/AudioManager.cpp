
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Audio/AudioManager.h"
#include "SpehsEngine/Audio/OpenALError.h"

#include <AL\al.h>
#include <vorbis\vorbisfile.h>

#include <functional>
#include <thread>


namespace audioVar
{
	std::mutex mutex;
	std::unordered_map<size_t, spehs::AudioManager::AudioClip> audioClips;
}

namespace spehs
{
	void AudioManager::init()
	{

	}
	void AudioManager::uninit()
	{
		deleteAllAudio();
	}

	
	ALuint AudioManager::isReady(const size_t& _hashID)
	{
		audioVar::mutex.lock();
		auto it = audioVar::audioClips.find(_hashID);
		if (it == audioVar::audioClips.end())
		{
			exceptions::warning("Sound doesn't exist!");
			audioVar::mutex.unlock();
			return 0;
		}
		ALuint value = 0;
		if (it->second.ready)
		{
			value = it->second.buffer;
		}
		audioVar::mutex.unlock();
		return value;
	}

	size_t AudioManager::load(const std::string& _filepath)
	{
		std::string fileEnding = _filepath.substr(_filepath.size() - 3);

		if (fileEnding == "wav")
		{
			return loadWAVE(_filepath);
		}
		else if (fileEnding == "ogg")
		{
			return loadOGG(_filepath);
		}
		else
		{
			exceptions::fatalError("Unsupported audio file!");
		}
		return 0;
	}
	size_t AudioManager::loadWAVE(const std::string &_filepath)
	{
		size_t hash = std::hash<std::string>()(_filepath);
		audioVar::mutex.lock();
		if (audioVar::audioClips.find(hash) != audioVar::audioClips.end())
		{
			audioVar::mutex.unlock();
			return hash;
		}

		AudioClip clip;
		audioVar::audioClips.insert(std::pair<size_t, AudioClip>(hash, clip));
		audioVar::mutex.unlock();
		std::thread bgLoaderThread(bgloadWAVE, _filepath);
		spehs::setThreadName(&bgLoaderThread, "Wave Loader Thread");
		bgLoaderThread.detach();
		return hash;
	}

	void AudioManager::bgloadWAVE(const std::string& _filepath)
	{
		FILE* fileData = nullptr;
		WAVE waveFile;
		audioVar::mutex.lock();
		auto it = audioVar::audioClips.find(std::hash<std::string>()(_filepath));
		audioVar::mutex.unlock();

		//Open file and make visual studio happy by doind it safely...
		errno_t error = fopen_s(&fileData, _filepath.c_str(), "rb");
		if (error != 0)
		{
			exceptions::fatalError("Failed to open WAVE file, error: " + std::to_string(error));
		}

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

		audioVar::mutex.lock();
		//The format is worked out by looking at the number of channels and the bits per sample
		if (waveFile.waveFormat.numChannels == 1)
		{
			if (waveFile.waveFormat.bitsPerSample == 8)
				it->second.format = AL_FORMAT_MONO8;
			else if (waveFile.waveFormat.bitsPerSample == 16)
				it->second.format = AL_FORMAT_MONO16;
			else
			{
				fclose(fileData);
				exceptions::fatalError("Bit depth of loaded mono audio file not supported! (File name: " + _filepath + " )");
			}
		}
		else if (waveFile.waveFormat.numChannels == 2)
		{
			if (waveFile.waveFormat.bitsPerSample == 8)
				it->second.format = AL_FORMAT_STEREO8;
			else if (waveFile.waveFormat.bitsPerSample == 16)
				it->second.format = AL_FORMAT_STEREO16;
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
		audioVar::mutex.unlock();

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
		audioVar::mutex.lock();
		it->second.size = waveFile.waveData.subChunkSize;
		it->second.freq = waveFile.waveFormat.sampleRate;
		audioVar::mutex.unlock();

		//Read sound data
		waveFile.data = new unsigned char[waveFile.waveData.subChunkSize];
		if (!fread(waveFile.data, waveFile.waveData.subChunkSize, 1, fileData))
		{
			fclose(fileData);
			exceptions::fatalError("Error loading WAVE sound file!");
		}

		audioVar::mutex.lock();
		//Generate OpenAL buffer
		checkOpenALErrors(__FILE__, __LINE__);
		alGenBuffers(1, &it->second.buffer);
		checkOpenALErrors(__FILE__, __LINE__);

		//Data into the buffer
		alBufferData(it->second.buffer, it->second.format, (void*) waveFile.data, it->second.size, it->second.freq);
		checkOpenALErrors(__FILE__, __LINE__);
		it->second.ready = true;
		audioVar::mutex.unlock();

		//Clean ups
		fclose(fileData);
		delete [] waveFile.data;
	}
	size_t AudioManager::loadOGG(const std::string& _filepath)
	{
		size_t hash = std::hash<std::string>()(_filepath);
		audioVar::mutex.lock();
		if (audioVar::audioClips.find(hash) != audioVar::audioClips.end())
		{
			audioVar::mutex.unlock();
			return hash;
		}

		AudioClip clip;
		audioVar::audioClips.insert(std::pair<size_t, AudioClip>(hash, clip));
		audioVar::mutex.unlock();
		std::thread bgLoaderThread(bgloadOGG, _filepath);
		spehs::setThreadName(&bgLoaderThread, "Ogg Loader Thread");
		bgLoaderThread.detach();
		return hash;
	}

	void AudioManager::bgloadOGG(const std::string& _filepath)
	{
		FILE* fileData = nullptr;
		audioVar::mutex.lock();
		auto it = audioVar::audioClips.find(std::hash<std::string>()(_filepath));
		audioVar::mutex.unlock();

		static const int endian = 0; //little is 0, big is 1; ogg should always be little-endian
		static const int BUFFER_SIZE = 32768;
		int bitStream;
		long bytes;
		char fixedBuffer[BUFFER_SIZE];
		std::vector<unsigned char> data;
		
		//Open file and make visual studio happy by doind it safely...
		errno_t error = fopen_s(&fileData, _filepath.c_str(), "rb");
		if (error != 0)
		{
			char errorString[100];
			strerror_s(errorString, 100, error);
			exceptions::fatalError("Failed to open OGG file: " + _filepath + "\n\terrno: " + std::to_string(error) + ", " + errorString);
		}

		vorbis_info* info;
		OggVorbis_File oggFile;

		ov_open(fileData, &oggFile, NULL, 0);

		//Get header info
		info = ov_info(&oggFile, -1);

		audioVar::mutex.lock();
		//Channels
		if (info->channels == 1)
		{
			it->second.format = AL_FORMAT_MONO16;
		}
		else
		{
			it->second.format = AL_FORMAT_STEREO16;
		}

		//Frequency
		it->second.freq = info->rate;
		audioVar::mutex.unlock();
		
		//Decoding
		do
		{
			//Read up to the buffers size of decoded data
			bytes = ov_read(&oggFile, fixedBuffer, BUFFER_SIZE, endian, 2, 1, &bitStream);

			//Add to end of data buffer
			data.insert(data.end(), fixedBuffer, fixedBuffer + bytes);
		} while (bytes > 0);

		ov_clear(&oggFile);
		//Apparently no need to call fclose

		audioVar::mutex.lock();
		it->second.size = ALsizei(data.size());

		//Generate OpenAL buffer
		alGenBuffers(1, &it->second.buffer);
		checkOpenALErrors(__FILE__, __LINE__);

		//Data into the buffer
		alBufferData(it->second.buffer, it->second.format, (void*) data.data(), it->second.size, it->second.freq);
		checkOpenALErrors(__FILE__, __LINE__);
		it->second.ready = true;
		audioVar::mutex.unlock();
	}
	size_t AudioManager::loadData(const std::string& _identifier, const unsigned char* _data, const int _size, const int _frequency, const AudioFormat _format)
	{
		audioVar::mutex.lock();
		size_t hash = std::hash<std::string>()(_identifier);
		if (audioVar::audioClips.find(hash) != audioVar::audioClips.end())
		{
			audioVar::mutex.unlock();
			return hash;
		}
		audioVar::mutex.unlock();

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

		audioVar::mutex.lock();
		audioVar::audioClips.insert(std::pair<size_t, AudioClip>(hash, clip));
		audioVar::mutex.unlock();
		return hash;
	}

	void AudioManager::deleteAudio(const std::string &_filepath)
	{
		deleteAudio(std::hash<std::string>()(_filepath));
	}
	void AudioManager::deleteAudio(const size_t &_hashID)
	{
		audioVar::mutex.lock();
		auto it = audioVar::audioClips.find(_hashID);
		if (it == audioVar::audioClips.end())
		{
			exceptions::fatalError("Trying to delete a WAVE file that doesn't exist!");
		}

		alDeleteBuffers(1, &it->second.buffer);

		audioVar::audioClips.erase(_hashID);
		audioVar::mutex.unlock();
	}
	void AudioManager::deleteAllAudio()
	{
		audioVar::mutex.lock();
		for (auto &it : audioVar::audioClips)
		{
			alDeleteBuffers(1, &it.second.buffer);
		}
		audioVar::audioClips.clear();
		audioVar::mutex.unlock();
	}

	AudioManager::AudioClip AudioManager::getAudioClip(const std::string _filepath)
	{
		return getAudioClip(std::hash<std::string>()(_filepath));
	}
	AudioManager::AudioClip AudioManager::getAudioClip(const size_t &_hashID)
	{
		std::lock_guard<std::mutex> amlock(audioVar::mutex);
		auto it = audioVar::audioClips.find(_hashID);
		if (it == audioVar::audioClips.end())
		{
			exceptions::fatalError("Trying to access non existent audio clip!");
		}

		return it->second;
	}
}