file(GLOB_RECURSE PFOPT_FILES "*.h" "*.hpp" "*.cpp")

if(MSVC)
    add_library(pfopt STATIC ${PFOPT_FILES})
    target_link_libraries(pfopt ifconsol)
    target_link_libraries(pfopt ifmodintr)
    target_link_libraries(pfopt libcoinblas)
    target_link_libraries(pfopt libcoinhsl)
    target_link_libraries(pfopt libcoinlapack)
    target_link_libraries(pfopt libcoinmetis)
    target_link_libraries(pfopt libifcoremt)
    target_link_libraries(pfopt libifport)
    target_link_libraries(pfopt libipopt)
    target_link_libraries(pfopt libirc)
    target_link_libraries(pfopt libmmt)
    target_link_libraries(pfopt svml_dispmt)
else()
    add_library(pfopt SHARED ${PFOPT_FILES})
    target_link_libraries(pfopt coinasl)
    target_link_libraries(pfopt coinhsl)
    target_link_libraries(pfopt coinmetis)
    target_link_libraries(pfopt coinmumps)
    target_link_libraries(pfopt ipopt)
    target_link_libraries(pfopt ipoptamplinterface)
    target_link_libraries(pfopt openblas)
endif()
