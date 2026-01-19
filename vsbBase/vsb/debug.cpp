//
// Created by Alexander on 19.01.2026.
//

#include "debug.h"


#ifdef VSB_SINGLE_THREAD_CHECK

#include <thread>

namespace vsb::debug
{
	void SingleThreadCheck(std::string_view detail)
	{
		static const std::thread::id threadId = std::this_thread::get_id();

		if (threadId != std::this_thread::get_id())
		{
			VSBLOG_ERROR("Single-thread violation! Expected thread: {}, Current thread: {}, Detail: {}",
						 std::hash<std::thread::id>{}(threadId),
						 std::hash<std::thread::id>{}(std::this_thread::get_id()),
						 detail);
			VSB_BREAK
		}
	}
}

#endif