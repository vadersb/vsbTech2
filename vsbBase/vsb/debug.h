//(C) 2025 Alexander Samarin

#pragma once

#include "log.h"
#include "vsb_settings.h"
#include <string_view>

#ifdef DEBUG

//break
#ifdef VSB_PLATFORM_WINDOWS

#ifdef _MSC_VER
#define VSB_BREAK __debugbreak();
#else
#define VSB_BREAK __builtin_trap();
#endif

#else
//todo platform support
#define VSB_BREAK (void)0;
#endif


//assert
#define VSB_ASSERT(x, msg)           if ((x)) {} else { VSBLOG_CRITICAL("ASSERT - {}\n\t{}\n\tin file: {}\n\ton line: {}", #x, msg, __FILE__, __LINE__); VSB_BREAK; }
#define VSB_ASSERT_V(x, msg, value1) if ((x)) {} else { VSBLOG_CRITICAL("ASSERT - {}\n\t{}\n\t{}: {}\n\tin file: {}\n\ton line: {}", #x, msg, #value1, value1, __FILE__, __LINE__); VSB_BREAK; }


#else

#define VSB_BREAK (void)0;
#define VSB_ASSERT(x, msg) (void)0;

#endif


// Single thread check
#ifdef VSB_SINGLE_THREAD_CHECK

namespace vsb::debug
{
	void SingleThreadCheck(std::string_view detail);
}

#define VSB_CHECK_THREAD() ::vsb::debug::SingleThreadCheck(__FILE__)

#else

#define VSB_CHECK_THREAD() (void)0

#endif