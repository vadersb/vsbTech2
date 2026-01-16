//
// Created by Alexander on 17.01.2026.
//


#pragma once
#include <vector>
#include "vsb/memory/single_threaded_allocator.h"


namespace vsb
{
	template<typename T>
	using vector = std::vector<T, memory::SingleThreadedAllocator<T>>;
}
