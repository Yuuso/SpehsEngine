#pragma once
#include "SpehsEngine/Core/FinalRelease.h"
#ifdef FINAL_RELEASE
//Generate no code!
#define se_pragma_optimize(p_value) ((void)0)
#else
#define se_pragma_optimize(p_value) __pragma(optimize("", p_value))
#endif
