cmake_minimum_required(VERSION 3.9)
project(seastar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(Boost_USE_STATIC_LIBS ON)

set(CMAKE_CPP_FLAGS "-Wall -DHAVE_HWLOC -DHAVE_NUMA -DHAVE_LZ4_COMPRESS_DEFAULT -DHAVE_ASAN_FIBER_SUPPORT")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_CPP_FLAGS}")

#set(CUSTOM_ROOT $ENV{HOME}/.root)
#list(APPEND CMAKE_FIND_ROOT_PATH ${CUSTOM_ROOT})

find_package(Boost COMPONENTS program_options filesystem thread system REQUIRED)

add_subdirectory(fmt)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/fmt
        ${Boost_INCLUDE_DIRS}
)

add_library(seastar_core
        core/app-template.cc
        core/dpdk_rte.cc
        core/exception_hacks.cc
        core/fsqual.cc
        core/fstream.cc
        core/future-util.cc
        core/linux-aio.cc
        core/memory.cc
        core/metrics.cc
        core/posix.cc
        core/reactor.cc
        core/resource.cc
        core/scollectd.cc
        core/systemwide_memory_barrier.cc
        core/thread.cc
        net/inet_address.cc
        net/net.cc
        net/packet.cc
        net/posix-stack.cc
        net/stack.cc
        rpc/lz4_compressor.cc
        rpc/rpc.cc
        util/alloc_failure_injector.cc
        util/backtrace.cc
        util/conversions.cc
        util/log.cc
        util/program-options.cc)

add_executable(hello_world apps/hello_world/hello_world.cc)
add_dependencies(hello_world seastar_core fmt::fmt)
target_link_libraries(hello_world seastar_core fmt::fmt ${Boost_LIBRARIES} rt hwloc numa pthread dl unwind)
