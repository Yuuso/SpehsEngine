#pragma once

// Standard library includes
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <set>
#include <stack>
#include <stdint.h>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Third party includes
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace se
{
	namespace time
	{
		class DeltaTimeSystem;
		class Time;
	}
	class ResourcePathFinder;
	template<typename T> class AsyncTaskManager;
	template<typename T> class WatchedValue;
	template<typename T> struct IStaticRingBuffer;
	template<typename T, uint32_t MaxSize> class StaticRingBuffer;
}

namespace boost
{
	namespace signals2
	{
		class scoped_connection;
	}
}

namespace std
{
	class thread;
}

/// Spehs Engine includes
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Constants.h"
#include "SpehsEngine/Core/RemoveCvref.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/Signal.h"
//
#include "SpehsEngine/Core/ByteVector.h"
#include "SpehsEngine/Core/ByteView.h"
#include "SpehsEngine/Core/ScopedConnections.h"
#include "SpehsEngine/Core/ScopedFrameLimiter.h"
#include "SpehsEngine/Core/ScopeProfiler.h"
//
#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Core/DeepPtrUtilityFunctions.h"
#include "SpehsEngine/Core/OptionalUtilityFunctions.h"
#include "SpehsEngine/Core/PairUtilityFunctions.h"
#include "SpehsEngine/Core/Serial/SerialUtility.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Core/TimeUtilityFunctions.h"
#include "SpehsEngine/Core/UniquePtrUtilityFunctions.h"
#include "SpehsEngine/Core/UnorderedMapUtilityFunctions.h"
#include "SpehsEngine/Core/UnorderedSetUtilityFunctions.h"
#include "SpehsEngine/Core/VectorUtilityFunctions.h"


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
