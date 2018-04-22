find_package(Protobuf REQUIRED)
file(GLOB_RECURSE protos
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        *.proto)
protobuf_generate_cpp(proto_srcs proto_hdrs ${protos})
add_library(seastar_protobuf ${proto_srcs})
target_include_directories(seastar_protobuf PUBLIC ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(seastar_protobuf ${Protobuf_LIBRARIES})