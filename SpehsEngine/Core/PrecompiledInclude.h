#pragma once

#include "glm/glm.hpp"
#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Constants.h"
#include "SpehsEngine/Core/Guid.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/ScopedFrameLimiter.h"
#include "SpehsEngine/Core/ScopeProfiler.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/SE_Time.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_map>
#include <stdint.h>
#include <string>
#include <vector>


#define SE_FALSE 1000
#define SE_TRUE 1001

// Platform
#define SE_PLATFORM_WINDOWS 2000
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define SE_PLATFORM SE_PLATFORM_WINDOWS
#else
#error Unknown platform
#endif

// Configuration
#define SE_CONFIGURATION_DEBUG 3001
#define SE_CONFIGURATION_RELEASE 3002
#define SE_CONFIGURATION_FINAL_RELEASE 3003
#ifdef _DEBUG
#define SE_CONFIGURATION SE_CONFIGURATION_DEBUG
#else
#define SE_CONFIGURATION SE_CONFIGURATION_RELEASE
// TODO: setting the configuration to final release
#endif

// Scope profiler
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
#define SE_ENABLE_SCOPE_PROFILER SE_TRUE
#else
#define SE_ENABLE_SCOPE_PROFILER SE_FALSE
#endif

namespace se
{
	class Archive;
}
