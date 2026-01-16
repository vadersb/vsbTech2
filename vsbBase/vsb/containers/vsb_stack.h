//
// Created by Alexander on 17.01.2026.
//

#pragma once

#include <stack>
#include "vsb/memory/single_threaded_allocator.h"

namespace vsb
{
	template<typename T>
	using stack = std::stack<T, std::vector<T, memory::SingleThreadedAllocator<T>>>;
}
