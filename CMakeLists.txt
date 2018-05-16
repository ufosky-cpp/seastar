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

#add_library(dpdk)
#target_link_libraries(dpdk -Wl,--whole-archive rte_pmd_vmxnet3_uio rte_pmd_i40e rte_pmd_ixgbe rte_pmd_e1000 rte_pmd_ring rte_pmd_bnxt rte_pmd_cxgbe rte_pmd_ena rte_pmd_enic rte_pmd_fm10k rte_pmd_nfp rte_pmd_qede rte_pmd_sfc_efx rte_hash rte_kvargs rte_mbuf rte_ethdev rte_eal rte_mempool rte_mempool_ring rte_ring rte_cmdline rte_cfgfile)

add_library(seastar_core
        core/alien.cc
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
target_compile_definitions(seastar_core PRIVATE -DHAVE_DPDK)
target_link_libraries(seastar_core
        -Wl,--whole-archive rte_pmd_vmxnet3_uio rte_pmd_i40e rte_pmd_ixgbe rte_pmd_e1000 rte_pmd_ring rte_pmd_bnxt rte_pmd_cxgbe rte_pmd_ena rte_pmd_enic rte_pmd_fm10k rte_pmd_nfp rte_pmd_qede rte_pmd_sfc_efx rte_hash rte_kvargs rte_mbuf rte_ethdev rte_eal rte_mempool rte_mempool_ring rte_ring rte_cmdline rte_cfgfile
        -Wl,--no-whole-archive fmt::fmt ${Boost_LIBRARIES} yaml-cpp pthread dl hwloc numa unwind rt lz4
        )
target_include_directories(seastar_core PUBLIC /usr/include/dpdk)

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
target_link_libraries(seastar_net
        -Wl,--no-whole-archive seastar_core c-ares::cares_static gnutls cryptopp)

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
target_link_libraries(seastar_http seastar_net)

add_library(seastar_prometheus
        core/prometheus.cc)
add_dependencies(seastar_prometheus seastar_core)
target_link_libraries(seastar_prometheus seastar_core)

add_subdirectory(proto)

add_library(seastar)
add_dependencies(seastar seastar_core seastar_net seastar_http seastar_prometheus seastar_protobuf)
target_link_libraries(seastar
        -Wl,--whole-archive seastar_net
        -Wl,--no-whole-archive seastar_core seastar_http seastar_prometheus seastar_protobuf)

add_executable(hello_world apps/hello_world/hello_world.cc)
add_dependencies(hello_world seastar)
target_link_libraries(hello_world seastar)

add_subdirectory(apps/httpd)
add_executable(httpd apps/httpd/main.cc)
add_dependencies(httpd seastar)
target_link_libraries(httpd
        -Wl,--whole-archive seastar)
