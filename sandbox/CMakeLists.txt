message(STATUS "sandbox samples included")

#basic

#01
add_executable(sandbox_basic01 basic/sandbox_basic01.cpp)
target_link_libraries(sandbox_basic01 vsbBase)

#02 - hashes
add_executable(sandbox_basic02_hashes basic/sandbox_basic02_hashes.cpp)
target_link_libraries(sandbox_basic02_hashes vsbBase)

#03 - memory alloc
add_executable(sandbox_basic03_memoryalloc basic/sandbox_basic03_memoryalloc.cpp)
target_link_libraries(sandbox_basic03_memoryalloc vsbBase)

#04 - inplace containers
add_executable(sandbox_basic04_inplace_containers basic/sandbox_basic04_inplace_containers.cpp)
target_link_libraries(sandbox_basic04_inplace_containers vsbBase)

#05 - object registry
add_executable(sandbox_basic05_object_registry basic/sandbox_basic05_object_registry.cpp)
target_link_libraries(sandbox_basic05_object_registry vsbBase)

#06 - object registry
add_executable(sandbox_basic06_delegates basic/sandbox_basic06_delegates.cpp)
target_link_libraries(sandbox_basic06_delegates vsbBase)