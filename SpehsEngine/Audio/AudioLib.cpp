#include "stdafx.h"
#include "SpehsEngine/Audio/AudioLib.h"


namespace se
{
	namespace
	{
		static int instanceCount = 0;
		static bool valid = false;
	}

	AudioLib::AudioLib()
	{
		instanceCount++;
		if (!valid)
		{
			valid = true;
		}
	}
	AudioLib::~AudioLib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}
	bool AudioLib::isValid()
	{
		return valid;
	}
}