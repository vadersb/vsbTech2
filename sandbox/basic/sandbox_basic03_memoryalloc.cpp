//(C) 2025 Alexander Samarin

#include "vsb/log.h"
#include "vsb/debug.h"
#include "vsb/memory/single_threaded_pool.h"
#include "vsb/objects/destruction_central.h"
#include "vsb/objects/internal/object_registry.h"


int main()
{
	vsb::VSBInit();

	const int Size = 128;

	auto* p1 = vsb::memory::SingleThreadedPool::Allocate(Size);

	VSBLOG_INFO("p1 address: {}", p1);

	vsb::memory::SingleThreadedPool::Deallocate(p1, Size);

	//auto* p2 = new int[16];

	//p2 not freed will cause assert failure in AllocationSystem::Uninit
	//auto p2 = vsb::memory::AllocationSystem::Allocate<vsb::memory::AllocationStrategy::StdChecked>(64);


	vsb::VSBUninit();
}
