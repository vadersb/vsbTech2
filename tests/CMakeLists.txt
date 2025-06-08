
# Find or add Catch2
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    # If not found, use FetchContent or add as subdirectory
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/libs/Catch2-3.8.1 catch2)
endif()


# Find all .cpp files in multiple directories and their subdirectories
file(GLOB_RECURSE TEST_SOURCE_FILES
        "base/*.cpp"
        "objects/*.cpp"
)


# Create test executable


add_executable(tests_vsbBase
        main.cpp
        ${TEST_SOURCE_FILES}
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