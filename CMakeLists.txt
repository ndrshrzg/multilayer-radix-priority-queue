cmake_minimum_required(VERSION 3.8)
project(multilayer_radix_priority_queue)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTXXL_PARALLEL=1")

#include STXXL
SET(TMP_CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
add_subdirectory(libs/stxxl/)
SET(CMAKE_BUILD_TYPE "${TMP_CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=10000")
include_directories(${STXXL_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -DSTXXL_VERBOSE_LEVEL=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSTXXL_VERBOSE_LEVEL=0")


set(SOURCE_FILES main.cpp multilayer_radix_pq.h)
add_executable(multilayer_radix_priority_queue ${SOURCE_FILES})
target_link_libraries(multilayer_radix_priority_queue ${STXXL_LIBRARIES})
