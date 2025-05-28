message(STATUS "sandbox samples included")

#basic

#01
add_executable(sandbox_basic01 basic/sandbox_basic01.cpp)
target_link_libraries(sandbox_basic01 vsbBase)

#02 - hashes
add_executable(sandbox_basic02_hashes basic/sandbox_basic02_hashes.cpp)
target_link_libraries(sandbox_basic02_hashes vsbBase)