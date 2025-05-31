//(C) 2025 Alexander Samarin

#pragma once

#include <atomic>

#include "allocation_strategy.h"
#include "vsb/debug.h"
#include "vsb/log.h"


namespace vsb::memory
{
	class AllocationSystem
	{
	public:

		AllocationSystem() = delete;

		//init/uninit
		static void Init();
		static void Uninit();

		//byte-based allocations
		template<AllocationStrategy allocationStrategy>
		static void* Allocate(const size_t size)
		{
			static_assert(allocationStrategy != AllocationStrategy::DefaultNewOperator, "for DefaultNewOperator operators shouldn't be declared in class");

			if (size == 0)
			{
				VSBLOG_CRITICAL("Zero size allocation requested. Allocation strategy: {}", static_cast<int>(allocationStrategy));
				return nullptr;
			}

			if constexpr (allocationStrategy == AllocationStrategy::Std)
			{
				return std::malloc(size);
			}

			if (s_state != State::Initialized)
			{
				VSBLOG_CRITICAL("Can't allocate: invalid system state.");
				return nullptr;
			}

			if constexpr (allocationStrategy == AllocationStrategy::SingleThreadedPool)
			{
				++s_allocations_SingleThreadedPool;
				return std::malloc(size); //todo hook the pool
			}

			if constexpr (allocationStrategy == AllocationStrategy::StdChecked)
			{
				++s_allocations_Std;
				return std::malloc(size);
			}

			VSBLOG_CRITICAL("Unhandled allocation strategy! {}", static_cast<int>(allocationStrategy));
			return nullptr;
		}


		template<AllocationStrategy allocationStrategy>
		static void Deallocate(void* &pointer, size_t size)
		{
			static_assert(allocationStrategy != AllocationStrategy::DefaultNewOperator, "for DefaultNewOperator operators shouldn't be declared in class");

			if (pointer == nullptr || size == 0)
			{
				VSBLOG_CRITICAL("provided pointer is null or size is 0");
				return;
			}

			if constexpr (allocationStrategy == AllocationStrategy::Std)
			{
				std::free(pointer);
				pointer = nullptr;
				return;
			}

			if (s_state != State::Initialized)
			{
				VSBLOG_CRITICAL("Can't allocate: invalid system state. allocation_strategy: {}", static_cast<int>(allocationStrategy));
				return;
			}

			if constexpr (allocationStrategy == AllocationStrategy::SingleThreadedPool)
			{
				--s_allocations_SingleThreadedPool;
				VSB_ASSERT(s_allocations_SingleThreadedPool >= 0, "");
				std::free(pointer); //todo hook the pool
			}

			if constexpr (allocationStrategy == AllocationStrategy::StdChecked)
			{
				--s_allocations_Std;
				VSB_ASSERT(s_allocations_Std >= 0, "");
				std::free(pointer);
			}

			pointer = nullptr;
		}


		//typed allocations
		template<typename T, AllocationStrategy allocationStrategy>
		static T* AllocateTyped(const size_t n)
		{
			static_assert(allocationStrategy != AllocationStrategy::DefaultNewOperator, "for DefaultNewOperator operators shouldn't be declared in class");
			static_assert(alignof(T) <= alignof(std::max_align_t));

			if (n == 0)
			{
				VSBLOG_CRITICAL("Zero size allocation requested. Allocation strategy: {}", static_cast<int>(allocationStrategy));
				return nullptr;
			}

			const size_t sizeInBytes = sizeof(T) * n;

			if constexpr (allocationStrategy == AllocationStrategy::Std)
			{
				return static_cast<T*>(std::malloc(sizeInBytes));
			}

			if (s_state != State::Initialized)
			{
				VSBLOG_CRITICAL("Can't allocate: invalid system state.");
				return nullptr;
			}

			if constexpr (allocationStrategy == AllocationStrategy::SingleThreadedPool)
			{
				++s_allocations_SingleThreadedPool;
				return static_cast<T*>(std::malloc(sizeInBytes)); //todo hook the pool
			}

			if constexpr (allocationStrategy == AllocationStrategy::StdChecked)
			{
				++s_allocations_Std;
				return static_cast<T*>(std::malloc(sizeInBytes));
			}

			VSBLOG_CRITICAL("Unhandled allocation strategy! {}", static_cast<int>(allocationStrategy));
			return nullptr;
		}


		template<typename T, AllocationStrategy allocationStrategy>
		static void DeallocateTyped(T* &pointer, const size_t n)
		{
			static_assert(allocationStrategy != AllocationStrategy::DefaultNewOperator, "for DefaultNewOperator operators shouldn't be declared in class");

			if (pointer == nullptr || n == 0)
			{
				VSBLOG_CRITICAL("provided pointer is null or n is 0");
				return;
			}

			if constexpr (allocationStrategy == AllocationStrategy::Std)
			{
				std::free(static_cast<void*>(pointer));
				pointer = nullptr;
				return;
			}

			if (s_state != State::Initialized)
			{
				VSBLOG_CRITICAL("Can't allocate: invalid system state. allocation_strategy: {}", static_cast<int>(allocationStrategy));
				return;
			}

			[[maybe_unused]]
			const size_t sizeInBytes = sizeof(T) * n;

			if constexpr (allocationStrategy == AllocationStrategy::SingleThreadedPool)
			{
				--s_allocations_SingleThreadedPool;
				std::free(static_cast<void*>(pointer));//todo hook the pool
			}

			if constexpr (allocationStrategy == AllocationStrategy::StdChecked)
			{
				--s_allocations_Std;
				std::free(static_cast<void*>(pointer));
			}

			pointer = nullptr;
		}

	private:

		enum class State
		{
			NotInitialized = 0,
			Initialized,
			Uninitialized,
		};

		static State s_state;

		static int s_allocations_SingleThreadedPool;
		static std::atomic_int s_allocations_Std;
	};
}
