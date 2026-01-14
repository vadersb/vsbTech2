//
// Created by Alexander on 14.01.2026.
//

#pragma once

#include "basic_memory_pool.h"


namespace flying_circles
{
    template<typename T>
    class BasicMemoryAllocator
    {
    public:

        using value_type = T;

        BasicMemoryAllocator() noexcept = default;

        template<typename U>
        BasicMemoryAllocator(const BasicMemoryAllocator<U>&) noexcept {}

        T* allocate(std::size_t n)
        {
            return static_cast<T*>(BasicMemoryPool::Allocate(n * sizeof(T)));
        }

        void deallocate(T* p, std::size_t n) noexcept
        {
            BasicMemoryPool::Deallocate(p, n * sizeof(T));
        }

        template<typename U>
        bool operator==(const BasicMemoryAllocator<U>&) const noexcept
        {
            return true;  // All instances are interchangeable (stateless allocator)
        }
    };

    template<typename T>
    using PooledVector = std::vector<T, BasicMemoryAllocator<T>>;

}