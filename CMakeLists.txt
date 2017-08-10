cmake_minimum_required (VERSION 2.8)

project(IpoptWrapper)

if(MSVC)
    set(BOOST_ROOT D:/dev/boost_1_64_0)
endif()
set(USE_STATIC_BOOST ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_STATIC_LIBS ON)


set(BIN_OUTOUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_OUTOUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_OUTOUT_DIRECTORY})
set(LIBRARY_OUTOUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTOUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTOUT_DIRECTORY})

find_package(Boost REQUIRED COMPONENTS chrono)
find_package(JNI)

if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif (Boost_FOUND)

if (JNI_FOUND)
    include_directories(${JNI_INCLUDE_DIRS})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/ipopt)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/eigen3)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/windows)
else()
    add_definitions(-std=c++11)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/linux)
endif()

add_subdirectory(pfopt)
add_subdirectory(java)