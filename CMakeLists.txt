# Andreas Herzog, 12/07/17
# andreas.herzog@stud.uni-frankfurt.de


cmake_minimum_required(VERSION 3.5)
project(multilayer_radix_priority_queue)

#INCLUDE TLX

add_subdirectory(tlx)
include_directories(tlx)


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTXXL_PARALLEL=1")

#INCLUDE STXXL
#SET(TMP_CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
SET(CMAKE_BUILD_TYPE "Debug")
add_subdirectory(libs/stxxl/)
#SET(CMAKE_BUILD_TYPE "${TMP_CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STXXL_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=10000")
include_directories(${STXXL_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -DSTXXL_VERBOSE_LEVEL=0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSTXXL_VERBOSE_LEVEL=0")

include_directories(libs/stxxl/include/)


#INCLUDE GTEST
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

add_executable(multilayer_radix_priority_queue main.cpp include/multilayer_radix_pq.h)
target_link_libraries(multilayer_radix_priority_queue ${STXXL_LIBRARIES})
#target_link_libraries(multilayer_radix_priority_queue tlx)
add_executable(benchmark_mlrpq_old benchmark.cpp include/multilayer_radix_pq.h)
target_link_libraries(benchmark_mlrpq_old ${STXXL_LIBRARIES})
#target_link_libraries(benchmark_mlrpq tlx)
add_executable(benchmark_mlrpq benchmark/mlrpq_benchmark.cpp benchmark/mlrpq_benchmark.h include/multilayer_radix_pq.h)
target_link_libraries(benchmark_mlrpq ${STXXL_LIBRARIES})
#target_link_libraries(benchmark_mlrpq tlx)
#add_executable(multilayer_radix_priority_queue_test test/multilayer_radix_pq_tests.cpp include/multilayer_radix_pq.h)
#target_link_libraries(multilayer_radix_priority_queue_test gtest_main ${STXXL_LIBRARIES})
#target_link_libraries(multilayer_radix_priority_queue_test gtest_main tlx)
