cmake_minimum_required(VERSION 3.10)
project(6_network_nods)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(ZMQ_INCLUDE_DIR /usr/local/include)

find_library(cZeroMQ NAMES zmq PATHS /usr/local/lib)
find_library(ZeroMQ NAMES zmqpp PATHS /usr/local/lib)


add_executable(main main.cpp BinTree.hpp server.hpp)
add_executable(node node.cpp BinTree.hpp server.hpp)


target_include_directories(main PUBLIC ${ZMQ_INCLUDE_DIR})
target_include_directories(node PUBLIC ${ZMQ_INCLUDE_DIR})


target_link_libraries(main ${ZeroMQ} ${cZeroMQ} Threads::Threads)
target_link_libraries(node ${ZeroMQ} ${cZeroMQ} Threads::Threads)
