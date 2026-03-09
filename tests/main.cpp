// main.cpp

#define CATCH_CONFIG_RUNNER  // Tell Catch2 we're providing our own main
#include <catch2/catch_all.hpp>

#include "vsb/log.h"
#include "vsb/objects/destruction_central.h"
#include "vsb/objects/singleton.h"
#include "vsb/objects/internal/object_registry.h"
#include "vsb/objects/internal/object_registry.h"

int main(int argc, char* argv[])
{
    // === SETUP ===
    vsb::log::InitForTests();

    VSBLOG_INFO("Global test setup init");

    vsb::VSBInit();

    // Run all Catch2 tests
    int const result = Catch::Session().run(argc, argv);

    // === TEARDOWN ===
    // This runs BEFORE any static destructors!
    VSBLOG_INFO("Global test setup uninit");
    vsb::VSBUninit();

    return result;
}
