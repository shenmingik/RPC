aux_source_directory(. SRC_LIST)
add_executable(provider ${SRC_LIST})
target_link_libraries(provider rpc protobuf)