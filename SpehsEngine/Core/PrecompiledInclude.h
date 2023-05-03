#pragma once

#include "boost/signals2/signal.hpp"
#include "boost/signals2/connection.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "SpehsEngine/Core/RemoveCvref.h"
#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/BufferUtilityFunctions.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Core/Constants.h"
#include "SpehsEngine/Core/DeepCopyUniquePtr.h"
#include "SpehsEngine/Core/DeepCopyUniquePtrUtilityFunctions.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/MultiMapUtilityFunctions.h"
#include "SpehsEngine/Core/ScopedFrameLimiter.h"
#include "SpehsEngine/Core/ScopeProfiler.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/STLOptionalUtilityFunctions.h"
#include "SpehsEngine/Core/STLPairUtilityFunctions.h"
#include "SpehsEngine/Core/STLUnorderedMapUtilityFunctions.h"
#include "SpehsEngine/Core/STLUnorderedSetUtilityFunctions.h"
#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Core/UniquePtrUtilityFunctions.h"
#include "SpehsEngine/Core/TimeUtilityFunctions.h"
#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/Serial/ArchiveReader.h"
#include "SpehsEngine/Core/Serial/ArchiveWriter.h"
#include <algorithm>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <numeric>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <optional>
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
// TODO: setting the configuration to final release nocommit
#endif

// Scope profiler
#if SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE
#define SE_ENABLE_SCOPE_PROFILER SE_TRUE
#else
#define SE_ENABLE_SCOPE_PROFILER SE_FALSE
#endif
