cmake_minimum_required(VERSION 3.0)
project(rpc)

# 设置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置可执行文件输出路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 配置子目录
add_subdirectory(src)