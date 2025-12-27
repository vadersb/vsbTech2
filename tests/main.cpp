//(C) 2025 Alexander Samarin

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "vsb/log.h"
#include "vsb/memory/memory.h"
#include "vsb/objects/internal/object_registry.h"
#include "vsb/objects/destruction_central.h"
#include "vsb/objects/singleton.h"

// Optional: Global test setup/teardown
struct GlobalTestSetup
{


	GlobalTestSetup()
	{
		vsb::log::InitForTests();
		VSBLOG_INFO("Global test setup init");
		vsb::memory::AllocationSystem::Init();
		objectRegistryFinalizer = std::make_unique<vsb::internal::ObjectRegistryFinalizer>();
	}

	~GlobalTestSetup()
	{
		VSBLOG_INFO("Global test setup uninit");
		vsb::DestructionCentral::Uninit();
		vsb::SingletonBase::DestroyAllSingletons();
		objectRegistryFinalizer.reset();
		vsb::memory::AllocationSystem::Uninit();
		vsb::log::Uninit();
	}

private:

	std::unique_ptr<vsb::internal::ObjectRegistryFinalizer> objectRegistryFinalizer;
};

static GlobalTestSetup g_testSetup;