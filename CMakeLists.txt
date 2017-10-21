cmake_minimum_required(VERSION 3.8)
project(multilayer_radix_priority_queue)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)

#add_subdirectory(stxxl)

#set(STXXL_DIR "cmake-build-debug/stxxl")
set(STXXL_DIR "/home/ndrshrzg/stxxl-1.4.1/build")
#set(STXXL_DIR "/home/ndrshrzg/Documents/stxxl/build")

find_package(STXXL REQUIRED)

# print some info (this can be removed)
message(STATUS "STXXL_CXX_FLAGS: ${STXXL_CXX_FLAGS}")
message(STATUS "STXXL_INCLUDE_DIRS: ${STXXL_INCLUDE_DIRS}")
message(STATUS "STXXL_LIBRARIES: ${STXXL_LIBRARIES}")

# apply CXXFLAGS to our configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")

# add STXXL include directory
include_directories(${STXXL_INCLUDE_DIRS})

# create and executable and linke with STXXL


set(SOURCE_FILES main.cpp multilayer_radix_pq.h)
add_executable(multilayer_radix_priority_queue ${SOURCE_FILES})
target_link_libraries(multilayer_radix_priority_queue ${STXXL_LIBRARIES})
