
# Find or add Catch2
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    # If not found, use FetchContent or add as subdirectory
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/libs/Catch2-3.8.1 catch2)
endif()

# Create test executable
add_executable(tests_vsbBase
        main.cpp
        base/tests_hash.cpp
        # unit/core/test_math.cpp
        # unit/core/test_utils.cpp
        # unit/graphics/test_renderer.cpp
        # Add more test files here
)

target_link_libraries(tests_vsbBase
        PRIVATE
        vsbBase
        Catch2::Catch2WithMain
)

target_include_directories(tests_vsbBase
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        fixtures/
)

# Enable CTest integration
include(CTest)
include(Catch)
catch_discover_tests(tests_vsbBase)