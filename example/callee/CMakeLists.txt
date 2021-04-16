aux_source_directory(. SRC_LIST)
aux_source_directory(../src PROTO_LIST)
add_executable(provider ${SRC_LIST} ${PROTO_LIST})
target_link_libraries(provider rpc protobuf muduo_base pthread zookeeper_mt)