//(C) 2025 Alexander Samarin

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/memory/memory.h"


int main()
{
	vsb::memory::AllocationSystem::Init();

	const int Size = 128;

	auto* p1 = vsb::memory::AllocationSystem::Allocate<vsb::memory::AllocationStrategy::StdChecked>(Size);

	VSBLOG_INFO("p1 address: {}", p1);

	vsb::memory::AllocationSystem::Deallocate<vsb::memory::AllocationStrategy::StdChecked>(p1, Size);

	auto* p2 = new int[16];

	//p2 not freed will cause assert failure in AllocationSystem::Uninit
	//auto p2 = vsb::memory::AllocationSystem::Allocate<vsb::memory::AllocationStrategy::StdChecked>(64);


	vsb::memory::AllocationSystem::Uninit();

}