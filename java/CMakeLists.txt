file(GLOB_RECURSE WRAPPER_FILES "*.h" "*.hpp" "*.cpp")
add_library(IpoptWrapper SHARED ${WRAPPER_FILES})

target_link_libraries(IpoptWrapper pfopt)