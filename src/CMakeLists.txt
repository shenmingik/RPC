aux_source_directory(. SRC_LIST)

add_library(rpc ${SRC_LIST})

target_link_libraries(rpc pthread muduo_base muduo_net zookeeper_mt)