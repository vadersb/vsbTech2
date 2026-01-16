//(C) 2025 Alexander Samarin

#pragma once

#include <cstdint>

namespace vsb::memory
{
	enum class AllocationStrategy : uint8_t
	{
		Std = 0,
		SingleThreadedPool,
		MultiThreadedPool,
	};

	constexpr AllocationStrategy DefaultAllocationStrategy = AllocationStrategy::SingleThreadedPool;
}