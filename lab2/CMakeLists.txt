cmake_minimum_required(VERSION 3.17)
project(OS_Labs C)

set(CMAKE_C_STANDARD 99)

add_executable(main main.c)

add_executable(child child.c)