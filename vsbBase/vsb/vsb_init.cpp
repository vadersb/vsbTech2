//
// Created by Alexander on 23.01.2026.
//


#include "vsb.h"
#include "memory/single_threaded_allocator.h"
#include "objects/destruction_central.h"
#include "objects/singleton.h"
#include "vsb/objects/internal/object_registry.h"

namespace vsb
{
	void VSBInit()
	{
		debug::SingleThreadCheckInit();
		memory::SingleThreadedPool::Init();
		internal::ObjectRegistry::Init();
		DestructionCentral::InitDefault();
	}


	void VSBUninit()
	{
		DestructionCentral::Uninit();
		SingletonBase::DestroyAllSingletons();
		internal::ObjectRegistry::WrapUp();
		vsb::log::Uninit();
	}
}
