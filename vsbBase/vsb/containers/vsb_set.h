//
// Created by Alexander on 17.01.2026.
//

#pragma once

#include <set>

#include "vsb/memory/single_threaded_allocator.h"

namespace vsb
{
	template<typename T, typename Compare = std::less<T>>
	using set = std::set<T, Compare, vsb::memory::SingleThreadedAllocator<T>>;
}
