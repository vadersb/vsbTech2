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


	struct FromEnd
	{
		Index offset;

		explicit FromEnd(Index theOffset, bool strict = false);
		[[nodiscard]] Index GetIndex(Count size) const noexcept;
	};
}
