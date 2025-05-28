message(STATUS "vsbTech2 global setup and options")

# platform
if(WIN32)
    add_definitions(-DVSB_PLATFORM_WINDOWS=1)
    message(STATUS "win32 platform")
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

# Compiler-specific flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
