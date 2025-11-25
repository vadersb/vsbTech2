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


	struct IndexEx
	{
	public:

		explicit(false) IndexEx(std::int64_t index, bool fromEnd = false)
		{
			m_index = index;
			m_fromEnd = fromEnd;
		}


		std::int64_t GetIndex(Count size) const noexcept
		{
			if (m_fromEnd)
			{
				return size - m_index - 1;
			}

			return m_index;
		}


		std::int64_t GetIndex(Count size, bool& isValid) const noexcept
		{
			std::int64_t localIndex = GetIndex(size);

			isValid = localIndex >= 0 && localIndex < size;

			return localIndex;
		}


	private:
		Index m_index = 0;
		bool m_fromEnd = false;
	};

}
