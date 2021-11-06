#include "stdafx.h"
#include "SpehsEngine/GUI/GUILib.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Audio/AudioLib.h"
#include "SpehsEngine/Graphics/GraphicsLib.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"

namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
	}

	GUILib::GUILib()
	{
		instanceCount++;
		if (!valid)
		{
			valid = true;
		}
	}

	GUILib::~GUILib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool GUILib::isValid()
	{
		return valid;
	}
}