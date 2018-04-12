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

set(CARES_STATIC ON CACHE BOOL "Build as a static library")

#set(CUSTOM_ROOT $ENV{HOME}/.root)
#list(APPEND CMAKE_FIND_ROOT_PATH ${CUSTOM_ROOT})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Boost COMPONENTS program_options filesystem thread system REQUIRED)
find_package(RAGEL REQUIRED)
find_package(Libunwind REQUIRED)

add_subdirectory(fmt)
add_subdirectory(c-ares)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/fmt
        ${CMAKE_SOURCE_DIR}/c-ares
        ${CMAKE_CURRENT_BINARY_DIR}/c-ares
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
add_dependencies(seastar_core fmt::fmt)

add_library(seastar_net
        net/proxy.cc
        net/virtio.cc
        net/dpdk.cc
        net/ip.cc
        net/ethernet.cc
        net/arp.cc
        net/native-stack.cc
        net/ip_checksum.cc
        net/udp.cc
        net/tcp.cc
        net/dhcp.cc
        net/tls.cc
        net/dns.cc
        net/config.cc)
add_dependencies(seastar_net seastar_core c-ares::cares_static)

add_subdirectory(http)
add_library(seastar_http
        http/transformers.cc
        http/json_path.cc
        http/file_handler.cc
        http/common.cc
        http/routes.cc
        json/json_elements.cc
        json/formatter.cc
        http/matcher.cc
        http/mime_types.cc
        http/httpd.cc
        http/reply.cc
        http/api_docs.cc)
add_dependencies(seastar_http seastar_net)

add_executable(hello_world apps/hello_world/hello_world.cc)
add_dependencies(hello_world seastar_core fmt::fmt)
target_link_libraries(hello_world seastar_core fmt::fmt ${Boost_LIBRARIES} rt hwloc numa pthread dl unwind)

add_subdirectory(apps/httpd)
add_executable(httpd apps/httpd/main.cc)
add_dependencies(httpd seastar_http)
target_link_libraries(httpd seastar_http seastar_net seastar_core fmt::fmt ${Boost_LIBRARIES} rt hwloc numa pthread dl unwind)
