#


add_executable(flying_circles)

#include(FetchContent)
#FetchContent_Declare(
#        raylib
#        GIT_REPOSITORY https://github.com/raysan5/raylib.git
#        GIT_TAG 5.5  # Use a stable tag or 'master' for latest; check https://github.com/raysan5/raylib/releases for versions
#        GIT_SHALLOW TRUE  # Optional: Shallow clone to speed up fetch
#)

set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)  # Disable examples to speed up build
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)  # Static lib for simplicity
# set(OPENGL_VERSION "3.3" CACHE STRING "" FORCE)  # Or "ES 2.0" if needed

#FetchContent_MakeAvailable(raylib)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/libs/raylib-5.5 raylib)

file(GLOB_RECURSE SOURCES "src/*.cpp")
target_sources(flying_circles PRIVATE ${SOURCES})

target_include_directories(flying_circles PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(flying_circles vsbBase raylib)