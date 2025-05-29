//(C) 2025 Alexander Samarin

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/memory/memory.h"


void main()
{
	vsb::memory::AllocationSystem::Init();

	auto p1 = vsb::memory::AllocationSystem::Allocate<vsb::memory::AllocationStrategy::StdChecked>(128);

	VSBLOG_INFO("p1 address: {}", reinterpret_cast<uintptr_t>(p1));

	vsb::memory::AllocationSystem::Deallocate<vsb::memory::AllocationStrategy::StdChecked>(p1, 128);


	//p2 not freed will cause assert failure in AllocationSystem::Uninit
	//auto p2 = vsb::memory::AllocationSystem::Allocate<vsb::memory::AllocationStrategy::StdChecked>(64);


	vsb::memory::AllocationSystem::Uninit();
}