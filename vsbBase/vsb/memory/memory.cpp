//(C) 2025 Alexander Samarin

#include "memory.h"

#include "vsb/debug.h"


namespace vsb::memory
{
	AllocationSystem::State AllocationSystem::s_state = State::NotInitialized;
	int AllocationSystem::s_allocations_SingleThreadedPool = 0;
	std::atomic_int AllocationSystem::s_allocations_Std = 0;


	void AllocationSystem::Init()
	{
		VSB_ASSERT(s_state == State::NotInitialized || s_state == State::Uninitialized, fmt::format("Invalid state: {}", static_cast<int>(s_state)));

		//todo pool init

		s_state = State::Initialized;
	}


	void AllocationSystem::Uninit()
	{
		VSB_ASSERT(s_state == State::Initialized, fmt::format("Invalid state: {}", static_cast<int>(s_state)));

		int stdAllocationsLeft = s_allocations_Std;
		VSB_ASSERT(stdAllocationsLeft == 0, fmt::format("Std allocations left: {}", stdAllocationsLeft));

		VSB_ASSERT(s_allocations_SingleThreadedPool == 0, fmt::format("SingleThreadedPool allocations left: {}", stdAllocationsLeft));

		//todo pool uninit

		s_state = State::Uninitialized;
	}
}
