//(C) 2025 Alexander Samarin

#pragma once


namespace vsb::memory
{
	enum class AllocationStrategy : uint8_t
	{
		Std = 0,
		StdChecked,
		SingleThreadedPool,
		DefaultNewOperator,
	};
}