add_library(vsbBase
        vsb/objects/ptr.h
        vsb/containers/inplace_array.h)

# Add all source files
file(GLOB_RECURSE SOURCES "vsb/*.cpp")
target_sources(vsbBase PRIVATE ${SOURCES})

# Set include directories
target_include_directories(vsbBase 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}  # This makes "vsb/..." work
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/single_header
)

# Set C++ standard
target_compile_features(vsbBase PUBLIC cxx_std_23)

target_link_libraries(vsbBase spdlog::spdlog)