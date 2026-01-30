//
// Created by Alexander on 19.01.2026.
//

#include "debug.h"


#ifdef VSB_SINGLE_THREAD_CHECK

#include <thread>

namespace vsb::debug
{
	static std::thread::id s_MainThreadID{};

	std::thread::id GetMainThreadID()
	{
		return s_MainThreadID;
	}


	void SingleThreadCheckInit()
	{
		s_MainThreadID = std::this_thread::get_id();
	}


	void SingleThreadCheck(std::string_view detail)
	{
		if (s_MainThreadID != std::this_thread::get_id())
		{
			VSBLOG_ERROR("Single-thread violation! Expected thread: {}, Current thread: {}, Detail: {}",
						 std::hash<std::thread::id>{}(s_MainThreadID),
						 std::hash<std::thread::id>{}(std::this_thread::get_id()),
						 detail);
			VSB_BREAK
		}
	}
}

#endif