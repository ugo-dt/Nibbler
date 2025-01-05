#pragma once

#include "Nibbler/Core/Core.hpp"

#ifdef NIB_ENABLE_ASSERTS
	#define NIB_ASSERT(expression, ...) \
		{ if(!(expression)) { \
			::Nibbler::Log::Error("Assertion failed: {0}:{1}: {2}", __FILE__, __LINE__, #expression); \
			::Nibbler::Log::Error("" __VA_ARGS__); exit(1); }}
#else
	#define NIB_ASSERT(expression, ...)
#endif
