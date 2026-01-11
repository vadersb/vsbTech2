//
// Created by Alexander on 22.12.2025.
//

#pragma once

#include <memory_resource>
#include <vector>

namespace vsb
{
	template<typename T>
	std::vector<T> ReserveEmptyVector(size_t size)
	{
		std::vector<T> result(size);
		result.reserve(size);
		return result;
	}


	template<typename T, size_t initialSize>
	class TempVectorSetup
	{
	public:
		// ReSharper disable once CppPossiblyUninitializedMember
		TempVectorSetup()
		{
			m_vector.reserve(initialSize);
		}

		std::pmr::vector<T>& GetVector() { return m_vector; }
		const std::pmr::vector<T>& GetVector() const { return m_vector; }

		void Clear() { m_vector.clear(); }

		TempVectorSetup(const TempVectorSetup&) = delete;
		TempVectorSetup& operator=(const TempVectorSetup&) = delete;
		TempVectorSetup(TempVectorSetup&&) = delete;
		TempVectorSetup& operator=(TempVectorSetup&&) = delete;

	private:

		static constexpr size_t BufferSize = sizeof(T) * initialSize + 64;

		alignas(std::max_align_t) std::byte m_buffer[BufferSize] ;

		std::pmr::monotonic_buffer_resource m_resource{ m_buffer, BufferSize };
		std::pmr::vector<T> m_vector{ &m_resource };
	};
}