//(C) 2025 Alexander Samarin

#pragma once
#include <cstdint>


namespace vsb
{
	using Index = std::int64_t;
	using Count = std::int64_t;

	constexpr Index InvalidIndex = -1;


	struct CapacitySetting
	{
		Count Capacity;
	};


	namespace internal
	{
		constexpr Index GetIndex(const Index index, const Count count, const bool fromEnd)
		{
			return fromEnd ? count - 1 - index : index;
		}
	}

}
