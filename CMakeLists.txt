cmake_minimum_required(VERSION 3.20)

# Project definition
project(vsbTech2 
    VERSION 0.1.0
    DESCRIPTION "vsbTech Mark 2"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # For IDE support

# Build options
option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_SAMPLES "Build sample projects" OFF)
option(BUILD_SANDBOX "Build sandbox project" OFF)
option(BUILD_TOOLS "Build development tools" OFF)

# Compiler-specific flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Debug/Release specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG=1)
    if(NOT MSVC)
        add_compile_options(-g -O0)
    endif()
else()
    add_compile_definitions(NDEBUG=1)
    if(NOT MSVC)
        add_compile_options(-O3)
    endif()
endif()

# Third-party dependencies
add_subdirectory(third_party)

# Core libraries
add_subdirectory(vsbBase)

# Optional components
if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()

# Install configuration
#include(GNUInstallDirs)

# Install headers
#install(DIRECTORY vsbBase/vsb/
#    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vsb
#    FILES_MATCHING PATTERN "*.h"
#)

#install(DIRECTORY vsbBase/vsb/
#    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vsb  
#    FILES_MATCHING PATTERN "*.h"
#)

# Install libraries
#nstall(TARGETS vsbBase # vsbGame
#    EXPORT MyGameFrameworkTargets
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)

# Export targets for find_package support
#install(EXPORT MyGameFrameworkTargets
#   FILE MyGameFrameworkTargets.cmake
#    NAMESPACE MyGameFramework::
#    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyGameFramework
#)

# Create config file for find_package
#include(CMakePackageConfigHelpers)
#configure_package_config_file(
#    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
#    "${CMAKE_CURRENT_BINARY_DIR}/MyGameFrameworkConfig.cmake"
#    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyGameFramework
#)

#write_basic_package_version_file(
#    "${CMAKE_CURRENT_BINARY_DIR}/vsbTech2ConfigVersion.cmake"
#    VERSION ${PROJECT_VERSION}
#    COMPATIBILITY AnyNewerVersion
#)

#install(FILES
#    "${CMAKE_CURRENT_BINARY_DIR}/vsbTech2Config.cmake"
#   "${CMAKE_CURRENT_BINARY_DIR}/vsbTech2ConfigVersion.cmake"
#    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vsbTech2
#)

# Print build configuration summary
message(STATUS "")
message(STATUS "vsbTech2 Build Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build samples: ${BUILD_SAMPLES}")
message(STATUS "  Build sandbox: ${BUILD_SANDBOX}")
message(STATUS "  Build tools: ${BUILD_TOOLS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")