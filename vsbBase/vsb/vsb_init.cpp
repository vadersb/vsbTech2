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
	static bool s_IsActive = false;

	void VSBInit()
	{
		s_IsActive = true;
#ifdef VSB_SINGLE_THREAD_CHECK
		debug::SingleThreadCheckInit();
#endif
		memory::SingleThreadedPool::Init();
		internal::ObjectRegistry::Init();
		DestructionCentral::InitDefault();
	}


	bool VSBIsActive()
	{
		return s_IsActive;
	}


	void VSBUninit()
	{
		DestructionCentral::Uninit();
		SingletonBase::DestroyAllSingletons();
		internal::ObjectRegistry::WrapUp();
		vsb::log::Uninit();
		s_IsActive = false;
	}
}
