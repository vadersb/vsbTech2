//(C) 2025 Alexander Samarin

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "vsb/log.h"
#include "vsb/memory/memory.h"
#include "vsb/objects/internal/object_registry.h"

// Optional: Global test setup/teardown
struct GlobalTestSetup
{
	vsb::internal::ObjectRegistryFinalizer* pObjectRegistryFinalizer;

	GlobalTestSetup()
	{
		vsb::log::InitForTests();
		VSBLOG_INFO("Global test setup init");
		vsb::memory::AllocationSystem::Init();
		pObjectRegistryFinalizer = new vsb::internal::ObjectRegistryFinalizer();
	}

	~GlobalTestSetup()
	{
		VSBLOG_INFO("Global test setup uninit");
		delete pObjectRegistryFinalizer;
		vsb::memory::AllocationSystem::Uninit();
		vsb::log::Uninit();
	}
};

static GlobalTestSetup g_testSetup;