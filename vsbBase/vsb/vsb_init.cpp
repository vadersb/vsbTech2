//
// Created by Alexander on 23.01.2026.
//


#include "vsb.h"
#include "memory/single_threaded_allocator.h"
#include "objects/destruction_central.h"
#include "vsb/objects/internal/object_registry.h"

namespace vsb
{
	void VSBInit()
	{
		memory::SingleThreadedPool::Init();
		internal::ObjectRegistry::Init();
		DestructionCentral::InitDefault();
	}
}