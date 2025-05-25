add_library(vsbBase)

# Add all source files
file(GLOB_RECURSE SOURCES "vsb/*.cpp")
target_sources(vsbBase PRIVATE ${SOURCES})

# Set include directories
target_include_directories(vsbBase 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}  # This makes "vsb/..." work
)

# Set C++ standard
target_compile_features(vsbBase PUBLIC cxx_std_23)