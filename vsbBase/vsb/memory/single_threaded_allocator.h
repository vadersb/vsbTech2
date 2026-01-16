//
// Created by Alexander on 17.01.2026.
//

#pragma once

#include "single_threaded_pool.h"

namespace vsb::memory
{
	template<typename T>
	class SingleThreadedAllocator
	{
	public:

		using value_type = T;

		SingleThreadedAllocator() noexcept = default;

		template<typename U>
		SingleThreadedAllocator(const SingleThreadedAllocator<U>&) noexcept {}

		T* allocate(std::size_t n)
		{
			return static_cast<T*>(SingleThreadedPool::Allocate(n * sizeof(T)));
		}

		void deallocate(T* p, std::size_t n) noexcept
		{
			SingleThreadedPool::Deallocate(p, n * sizeof(T));
		}

		template<typename U>
		bool operator==(const SingleThreadedAllocator<U>&) const noexcept
		{
			return true;  // All instances are interchangeable (stateless allocator)
		}
	};
}