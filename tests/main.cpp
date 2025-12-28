// main.cpp

#define CATCH_CONFIG_RUNNER  // Tell Catch2 we're providing our own main
#include <catch2/catch_all.hpp>

#include "vsb/log.h"
#include "vsb/memory/memory.h"
#include "vsb/objects/destruction_central.h"
#include "vsb/objects/singleton.h"
#include "vsb/objects/internal/object_registry.h"

int main(int argc, char* argv[])
{
    // === SETUP ===
    vsb::log::InitForTests();
    VSBLOG_INFO("Global test setup init");
    vsb::memory::AllocationSystem::Init();

    // Run all Catch2 tests
    int const result = Catch::Session().run(argc, argv);

    // === TEARDOWN ===
    // This runs BEFORE any static destructors!
    VSBLOG_INFO("Global test setup uninit");
    vsb::DestructionCentral::Uninit();
    vsb::SingletonBase::DestroyAllSingletons();

    {
        vsb::internal::ObjectRegistryFinalizer const finalizer;
    }

    vsb::memory::AllocationSystem::Uninit();
    vsb::log::Uninit();

    return result;
}
